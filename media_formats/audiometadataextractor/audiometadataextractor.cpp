#include <iostream>
#include <filesystem>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/audioproperties.h>
#include <string>

namespace fs = std::filesystem;

// AudioMetadataExtractor class scans a directory and extracts metadata from audio files using TagLib
class AudioMetadataExtractor
{
public:
    // Scans the given directory (non-recursively) and processes each regular file
    void scanDirectory(const std::string& path)
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (fs::is_regular_file(entry))
            {
                processFile(entry.path().string());
            }
        }
    }

private:
    // Processes a single file: extracts and prints audio metadata
    void processFile(const std::string& filepath)
    {
        TagLib::FileRef file(filepath.c_str());
        if (!file.isNull() && file.tag() && file.audioProperties())
        {
            // Retrieve tag and audio properties pointers
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
            std::cerr << "Error: Could not read metadata for " << filepath << "\n";
        }
    }
};

int main()
{
    std::string directory;
    std::cout << "Enter the directory containing audio files: ";
    std::getline(std::cin, directory);

    AudioMetadataExtractor extractor;
    extractor.scanDirectory(directory);

    return 0;
}
