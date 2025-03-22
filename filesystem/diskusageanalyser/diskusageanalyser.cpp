#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

namespace fs = std::filesystem;

// DiskUsageAnalyser class scans directories recursively and displays file sizes
// It paginates the output to show a limited number of files at a time
class DiskUsageAnalyser
{
public:
    // Scans the given directory, collects file paths and sizes, sorts them,
    // and displays them in a paginated manner
    void analyseDirectory(const std::string& path)
    {
        std::vector<std::pair<std::string, uintmax_t>> files;

        // Recursively iterate through the directory and collect file data
        try
        {
            for (const auto& entry : fs::recursive_directory_iterator(path))
            {
                if (fs::is_regular_file(entry))
                {
                    files.emplace_back(entry.path().string(), fs::file_size(entry));
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::cerr << "Error accessing file: " << e.what() << std::endl;
            return;
        }

        // Check if any files were found
        if (files.empty())
        {
            std::cout << "No files found in the directory." << std::endl;
            return;
        }

        // Sort files in descending order by size
        std::sort(files.begin(), files.end(), [](const auto& a, const auto& b)
        {
            return a.second > b.second;
        });

        // Display files in pages
        displayFilesPaginated(files);
    }

private:
    // Displays file information in a paginated table format
    // Each page shows 'pageSize' files. User can press Enter to continue or type 'q' to quit
    void displayFilesPaginated(const std::vector<std::pair<std::string, uintmax_t>>& files, size_t pageSize = 10)
    {
        size_t totalFiles = files.size();
        size_t index = 0;

        // Continue until all files are displayed or user quits
        while (index < totalFiles)
        {
            // Construct horizontal separator line (adjusted to match column widths)
            constexpr int lineLength = 66;
            std::cout << "\n" << std::string(lineLength, '-') << std::endl;
            std::cout << "| " << std::setw(50) << std::left << "File Path"
                      << " | " << std::setw(10) << "Size (KB)" << " |" << std::endl;
            std::cout << std::string(lineLength, '-') << std::endl;

            // Display one page of files
            for (size_t i = 0; i < pageSize && index < totalFiles; ++i, ++index)
            {
                // Truncate the file path to 50 characters if needed
                std::string filePath = files[index].first.substr(0, 50);
                // Convert size to KB
                uintmax_t sizeKB = files[index].second / 1024;

                std::cout << "| " << std::setw(50) << std::left << filePath
                          << " | " << std::setw(10) << sizeKB << " |" << std::endl;
            }

            std::cout << std::string(lineLength, '-') << std::endl;

            // If more files remain, prompt the user to continue
            if (index < totalFiles)
            {
                std::cout << "Press Enter to see more, or type 'q' to quit: ";
                std::string input;
                std::getline(std::cin, input);
                if (input == "q")
                {
                    break;
                }
            }
        }
    }
};

int main()
{
    std::string path;
    std::cout << "Enter directory to analyse: ";
    std::getline(std::cin, path);

    DiskUsageAnalyser analyser;
    analyser.analyseDirectory(path);

    return 0;
}
