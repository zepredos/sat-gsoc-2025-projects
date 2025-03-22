#include <iostream>
#include <filesystem>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <sstream>
#include <string>
#include <cstdlib>
#include <poll.h>
#include <fcntl.h>
#include <chrono>

extern "C"
{
    #include <sqlite3.h>
    #include <sys/inotify.h>
    #include <unistd.h>
    #include <cstring>
}

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------
// ThreadSafeQueue: A simple thread-safe queue template
template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condVar;
public:
    void push(const T& item)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(item);
        }
        m_condVar.notify_one();
    }
    bool try_pop(T& item, int timeout_ms)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_condVar.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                  [this] { return !m_queue.empty(); }))
        {
            return false;
        }
        item = m_queue.front();
        m_queue.pop();
        return true;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
};

//------------------------------------------------------------------------------
// MediaMetadata: Holds metadata extracted from a media file
struct MediaMetadata
{
    std::unordered_map<std::string, std::string> m_data;
};

//------------------------------------------------------------------------------
// InotifyFileScanner: Uses inotify (with poll in non-blocking mode) to monitor a directory for file creation events
class InotifyFileScanner
{
private:
    int m_inotifyFd{ -1 };
    ThreadSafeQueue<std::string>& m_queue;
    std::string m_directory;
    bool m_running{ true };
    bool* m_pScanningDone;
public:
    InotifyFileScanner(const std::string &directory,
                       ThreadSafeQueue<std::string>& q,
                       bool* pScanningDone)
        : m_queue(q), m_directory(directory), m_pScanningDone(pScanningDone)
    {
        m_inotifyFd = inotify_init1(IN_NONBLOCK);
        if (m_inotifyFd < 0)
        {
            perror("inotify_init1");
            exit(EXIT_FAILURE);
        }
    }
    ~InotifyFileScanner()
    {
        if (m_inotifyFd >= 0)
            close(m_inotifyFd);
    }
    void start()
    {
        int wd = inotify_add_watch(m_inotifyFd, m_directory.c_str(), IN_CREATE);
        if (wd < 0)
        {
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
        constexpr size_t eventSize = sizeof(struct inotify_event);
        constexpr size_t bufLen = 1024 * (eventSize + 16);
        char buffer[bufLen];
        struct pollfd fds[1];
        fds[0].fd = m_inotifyFd;
        fds[0].events = POLLIN;
        int timeoutCount = 0;
        const int maxTimeoutCount = 20; // 2000ms total
        while (m_running)
        {
            if (m_pScanningDone && *m_pScanningDone && m_queue.empty())
            {
                timeoutCount++;
                if (timeoutCount >= maxTimeoutCount)
                    break;
            }
            else
            {
                timeoutCount = 0;
            }
            int pollNum = poll(fds, 1, 100);
            if (pollNum < 0)
            {
                perror("poll");
                exit(EXIT_FAILURE);
            }
            if (pollNum == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            else if (pollNum > 0)
            {
                int length = read(m_inotifyFd, buffer, bufLen);
                if (length < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    perror("read");
                    exit(EXIT_FAILURE);
                }
                for (int i = 0; i < length; )
                {
                    struct inotify_event* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);
                    if (event->len && (event->mask& IN_CREATE))
                    {
                        std::string filePath = m_directory + "/" + event->name;
                        if (fs::is_regular_file(filePath))
                        {
                            m_queue.push(filePath);
                        }
                    }
                    i += eventSize + event->len;
                }
            }
        }
        inotify_rm_watch(m_inotifyFd, wd);
    }
    void stop()
    {
        m_running = false;
    }
};

//------------------------------------------------------------------------------
// MetadataExtractorWorker: Processes file paths from the queue, extracts metadata, and stores the results
class MetadataExtractorWorker
{
private:
    ThreadSafeQueue<std::string>& m_queue;
    std::unordered_map<std::string, MediaMetadata>& m_metadataStore;
    std::mutex& m_storeMutex;
    bool& m_scanningDone;
public:
    MetadataExtractorWorker(ThreadSafeQueue<std::string>& q,
                            std::unordered_map<std::string, MediaMetadata>& store,
                            std::mutex& mtx,
                            bool& done)
        : m_queue(q), m_metadataStore(store), m_storeMutex(mtx), m_scanningDone(done)
    {
    }
    void operator()()
    {
        while (true)
        {
            std::string filepath;
            if (m_queue.try_pop(filepath, 100))
            {
                MediaMetadata meta = extractMetadata(filepath);
                {
                    std::lock_guard<std::mutex> lock(m_storeMutex);
                    m_metadataStore[filepath] = meta;
                }
            }
            else if (m_queue.empty() && m_scanningDone)
            {
                break;
            }
        }
    }
    MediaMetadata extractMetadata(const std::string& filepath)
    {
        MediaMetadata meta;
        fs::path p(filepath);
        std::string ext = p.extension().string();
        if (ext == ".mp3" || ext == ".wav")
        {
            TagLib::FileRef file(filepath.c_str());
            if (!file.isNull() && file.tag() && file.audioProperties())
            {
                auto* tag = file.tag();
                auto* properties = file.audioProperties();
                meta.m_data["Type"] = "Audio";
                meta.m_data["Artist"] = tag->artist().to8Bit(true);
                meta.m_data["Album"] = tag->album().to8Bit(true);
                meta.m_data["Title"] = tag->title().to8Bit(true);
                meta.m_data["Year"] = std::to_string(tag->year());
                meta.m_data["Duration"] = std::to_string(properties->lengthInSeconds());
            }
            else
            {
                meta.m_data["Type"] = "Audio";
                meta.m_data["Error"] = "Metadata extraction failed";
            }
        }
        else if (ext == ".mp4" || ext == ".avi")
        {
            meta.m_data["Type"] = "Video";
            meta.m_data["Resolution"] = "1920x1080";
            meta.m_data["Duration"] = "300";
        }
        else if (ext == ".vst" || ext == ".dll")
        {
            meta.m_data["Type"] = "VST Plugin";
            meta.m_data["Version"] = "1.0";
        }
        else if (ext == ".preset")
        {
            meta.m_data["Type"] = "Preset";
        }
        else
        {
            meta.m_data["Type"] = "Other";
        }
        return meta;
    }
};

//------------------------------------------------------------------------------
// LibraryContentManager: Coordinates scanning, metadata extraction, database storage, and custom tagging
class LibraryContentManager
{
private:
    ThreadSafeQueue<std::string> m_fileQueue;
    std::unordered_map<std::string, MediaMetadata> m_metadataStore;
    std::unordered_map<std::string, std::vector<std::string>> m_customTags;
    std::mutex m_storeMutex;
    bool m_scanningDone { false };
    sqlite3* m_db { nullptr };

    void openDatabase()
    {
        if (sqlite3_open("library.db", &m_db) != SQLITE_OK)
        {
            std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << "\n";
            exit(EXIT_FAILURE);
        }
        const char* createTableSQL =
            "CREATE TABLE IF NOT EXISTS media_metadata ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "filepath TEXT UNIQUE, "
            "type TEXT, "
            "artist TEXT, "
            "album TEXT, "
            "title TEXT, "
            "year TEXT, "
            "duration TEXT);";
        char* errMsg = nullptr;
        if (sqlite3_exec(m_db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::cerr << "SQL error: " << errMsg << "\n";
            sqlite3_free(errMsg);
            exit(EXIT_FAILURE);
        }
    }

    void insertMetadata(const std::string& filepath, const MediaMetadata& meta)
    {
        std::string sql = "INSERT OR REPLACE INTO media_metadata (filepath, type, artist, album, title, year, duration) VALUES ("
                          "'" + filepath + "', "
                          "'" + (meta.m_data.count("Type") ? meta.m_data.at("Type") : "") + "', "
                          "'" + (meta.m_data.count("Artist") ? meta.m_data.at("Artist") : "") + "', "
                          "'" + (meta.m_data.count("Album") ? meta.m_data.at("Album") : "") + "', "
                          "'" + (meta.m_data.count("Title") ? meta.m_data.at("Title") : "") + "', "
                          "'" + (meta.m_data.count("Year") ? meta.m_data.at("Year") : "") + "', "
                          "'" + (meta.m_data.count("Duration") ? meta.m_data.at("Duration") : "") + "');";
        char* errMsg = nullptr;
        if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::cerr << "SQL error on insert: " << errMsg << "\n";
            sqlite3_free(errMsg);
        }
    }

    void queryDatabaseImpl() const
    {
        const char* sql = "SELECT filepath, type, artist, album, title, year, duration FROM media_metadata;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        {
            std::cerr << "Failed to prepare query: " << sqlite3_errmsg(m_db) << "\n";
            return;
        }
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::cout << "File: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) << "\n";
            std::cout << "  Type: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) << "\n";
            std::cout << "  Artist: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) << "\n";
            std::cout << "  Album: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) << "\n";
            std::cout << "  Title: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) << "\n";
            std::cout << "  Year: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) << "\n";
            std::cout << "  Duration: " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) << "\n";
            std::cout << "---------------------------------------\n";
        }
        sqlite3_finalize(stmt);
    }

    void closeDatabase()
    {
        if (m_db)
        {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

public:
    LibraryContentManager() { }

    ~LibraryContentManager() { closeDatabase(); }

    void run(const std::string& directory)
    {
        if (!fs::exists(directory) || !fs::is_directory(directory))
        {
            std::cerr << "Error: Directory does not exist or is invalid.\n";
            exit(EXIT_FAILURE);
        }

        openDatabase();

        InotifyFileScanner scanner(directory, m_fileQueue, &m_scanningDone);
        std::thread scannerThread(&InotifyFileScanner::start, &scanner);

        unsigned int numWorkers = std::thread::hardware_concurrency();
        if (numWorkers == 0)
            numWorkers = 2;
        std::vector<std::thread> workers;
        for (unsigned int i = 0; i < numWorkers; ++i)
        {
            workers.emplace_back(MetadataExtractorWorker(m_fileQueue, m_metadataStore, m_storeMutex, m_scanningDone));
        }

        try
        {
            for (const auto& entry : fs::recursive_directory_iterator(directory))
            {
                if (fs::is_regular_file(entry))
                {
                    m_fileQueue.push(entry.path().string());
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error scanning directory: " << e.what() << "\n";
            exit(EXIT_FAILURE);
        }

        {
            std::lock_guard<std::mutex> lock(m_storeMutex);
            m_scanningDone = true;
        }

        for (auto& worker : workers)
        {
            worker.join();
        }

        for (const auto& pair : m_metadataStore)
        {
            insertMetadata(pair.first, pair.second);
        }

        scanner.stop();
        scannerThread.join();
    }

    void viewLibrary() const
    {
        std::cout << "\nLibrary Content Manager - Media Metadata:\n";
        for (const auto& pair : m_metadataStore)
        {
            std::cout << "File: " << pair.first << "\n";
            for (const auto& meta : pair.second.m_data)
            {
                std::cout << "  " << meta.first << " : " << meta.second << "\n";
            }
            std::cout << "---------------------------------------\n";
        }
    }

    void displayDatabase() const
    {
        std::cout << "\nDatabase Contents:\n";
        queryDatabaseImpl();
    }

    void addCustomTag(const std::string& filepath, const std::string& tag)
    {
        std::lock_guard<std::mutex> lock(m_storeMutex);
        m_customTags[filepath].push_back(tag);
    }

    void viewCustomTags(const std::string& filepath) const
    {
        auto it = m_customTags.find(filepath);
        if (it != m_customTags.end())
        {
            std::cout << "Custom Tags for " << filepath << ":\n";
            for (const auto& tag : it->second)
            {
                std::cout << "  " << tag << "\n";
            }
        }
        else
        {
            std::cout << "No custom tags for " << filepath << ".\n";
        }
    }
};

//------------------------------------------------------------------------------
// Simple CLI interface for interacting with LibraryContentManager
void cliInterface(LibraryContentManager& lcm)
{
    std::string line;
    std::cout << "\nEnter a command (type 'help' for a list of commands):\n";
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        if (command == "help")
        {
            std::cout << "Available commands:\n";
            std::cout << "  list                : View in-memory metadata.\n";
            std::cout << "  db                  : Display database contents.\n";
            std::cout << "  tag <filepath> <tag>: Add a custom tag to a file.\n";
            std::cout << "  viewtag <filepath>  : View custom tags for a file.\n";
            std::cout << "  exit                : Exit the program.\n";
        }
        else if (command == "list")
        {
            lcm.viewLibrary();
        }
        else if (command == "db")
        {
            lcm.displayDatabase();
        }
        else if (command == "tag")
        {
            std::string filepath, tag;
            iss >> filepath >> tag;
            if (!filepath.empty() && !tag.empty())
            {
                lcm.addCustomTag(filepath, tag);
                std::cout << "Tag added.\n";
            }
            else
            {
                std::cout << "Usage: tag <filepath> <tag>\n";
            }
        }
        else if (command == "viewtag")
        {
            std::string filepath;
            iss >> filepath;
            if (!filepath.empty())
            {
                lcm.viewCustomTags(filepath);
            }
            else
            {
                std::cout << "Usage: viewtag <filepath>\n";
            }
        }
        else if (command == "exit")
        {
            break;
        }
        else
        {
            std::cout << "Unknown command. Type 'help' for a list of available commands.\n";
        }
    }
}

int main()
{
    LibraryContentManager lcm;
    std::string directory;

    std::cout << "Enter directory to scan for media files: ";
    std::getline(std::cin, directory);

    lcm.run(directory);

    // Instead of printing all metadata immediately, print a summary message
    std::cout << "\nScan complete. Records have been stored in the database.\n";
    std::cout << "Enter 'help' for a list of available commands.\n";

    // Start CLI command loop
    cliInterface(lcm);

    return 0;
}
