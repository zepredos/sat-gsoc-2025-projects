#include <iostream>
#include <filesystem>
#include <unordered_map>

namespace fs = std::filesystem;

// FileOrganiser class scans a directory and moves files into categorized folders
class FileOrganiser
{
private:
    // Mapping file extensions to folder names
    std::unordered_map<std::string, std::string> categoryMap =
    {
        { ".jpg", "Images" },
        { ".png", "Images" },
        { ".gif", "Images" },
        { ".bmp", "Images" },
        { ".txt", "Documents" },
        { ".pdf", "Documents" },
        { ".docx", "Documents" },
        { ".xlsx", "Documents" },
        { ".mp4", "Videos" },
        { ".mkv", "Videos" },
        { ".avi", "Videos" },
        { ".mp3", "Audio" },
        { ".wav", "Audio" },
        { ".flac", "Audio" },
        { ".zip", "Archives" },
        { ".rar", "Archives" },
        { ".tar", "Archives" }
    };

    // Returns the category based on file extension
    std::string getCategory(const fs::path& filePath)
    {
        std::string ext = filePath.extension().string();
        if (categoryMap.find(ext) != categoryMap.end())
        {
            return categoryMap[ext];
        }
        return "Others";
    }

public:
    // Organises files in the given directory into categorised subfolders
    void organise(const std::string& directory)
    {
        if (!fs::exists(directory) || !fs::is_directory(directory))
        {
            std::cerr << "Invalid directory path!" << std::endl;
            return;
        }

        // Iterate over each file in the directory
        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (fs::is_regular_file(entry.path()))
            {
                std::string category = getCategory(entry.path());
                fs::path targetDir = fs::path(directory) / category;

                // Create target directory if it doesn't exist
                fs::create_directories(targetDir);

                fs::path targetPath = targetDir / entry.path().filename();

                // Move the file to its target directory
                fs::rename(entry.path(), targetPath);

                std::cout << "Moved: " << entry.path().filename()
                          << " -> " << category << "/" << std::endl;
            }
        }

        std::cout << "Organisation complete." << std::endl;
    }
};

int main()
{
    std::string directory;
    std::cout << "Enter directory to organise: ";
    std::getline(std::cin, directory);

    FileOrganiser organiser;
    organiser.organise(directory);

    return 0;
}
