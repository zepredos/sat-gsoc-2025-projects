#include <iostream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <string>

namespace fs = std::filesystem;

// Mutex to synchronise console output
std::mutex coutMutex;

// Recursively scans the specified directory and prints the paths of all regular files
// If an error occurs (e.g., permission issues), an error message is printed
void scan_directory(const std::string& path)
{
    try
    {
        for (const auto& entry : fs::recursive_directory_iterator(path))
        {
            if (fs::is_regular_file(entry))
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "File: " << entry.path() << "\n";
            }
        }
    }
    catch (const fs::filesystem_error & e)
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "Error scanning directory " << path << ": " << e.what() << "\n";
    }
}

int main()
{
    std::vector<std::string> directories;
    std::string line;

    std::cout << "Enter directories to scan (separated by spaces): ";
    std::getline(std::cin, line);

    std::istringstream iss(line);
    std::string token;
    while (iss >> token)
    {
        directories.push_back(token);
    }

    std::vector<std::thread> threads;
    for (const auto& dir : directories)
    {
        threads.emplace_back(scan_directory, dir);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
