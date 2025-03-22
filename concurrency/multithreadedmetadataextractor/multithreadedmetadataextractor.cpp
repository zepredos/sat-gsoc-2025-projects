#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <string>

namespace fs = std::filesystem;

// Mutex to synchronise console output
std::mutex outputMutex;

// MetadataExtractor class scans a directory and extracts audio metadata from files concurrently
class MetadataExtractor
{
public:
    // Scans the given directory and spawns a thread for each regular file to process metadata
    void scan_directory(const std::string& path)
    {
        std::vector<std::thread> threads;

        for (const auto& entry : fs::directory_iterator(path))
        {
            if (fs::is_regular_file(entry))
            {
                threads.emplace_back(&MetadataExtractor::process_file, this, entry.path().string());
            }
        }

        for (auto& t : threads)
        {
            t.join();
        }
    }

private:
    // Processes a single file by extracting and printing its audio metadata
    void process_file(const std::string& filepath)
    {
        TagLib::FileRef file(filepath.c_str());
        if (!file.isNull() && file.tag() && file.audioProperties())
        {
            // Lock output to avoid interleaving output from multiple threads
            std::lock_guard<std::mutex> lock(outputMutex);
            auto* tag = file.tag();
            auto* properties = file.audioProperties();

            std::cout << "File: " << filepath << "\n";
            std::cout << "Artist: " << tag->artist().to8Bit(true) << "\n";
            std::cout << "Album: " << tag->album().to8Bit(true) << "\n";
            std::cout << "Title: " << tag->title().to8Bit(true) << "\n";
            std::cout << "Year: " << tag->year() << "\n";
            std::cout << "Duration: " << properties->lengthInSeconds() << " sec\n";
            std::cout << "---------------------------------------\n";
        }
        else
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cerr << "Error: Could not read metadata for " << filepath << "\n";
        }
    }
};

int main()
{
    std::string directory;
    std::cout << "Enter the directory containing media files: ";
    std::getline(std::cin, directory);

    MetadataExtractor extractor;
    extractor.scan_directory(directory);

    return 0;
}
