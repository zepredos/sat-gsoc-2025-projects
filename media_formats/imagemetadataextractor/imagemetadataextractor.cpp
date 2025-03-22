#include <iostream>
#include <filesystem>
#include <exiv2/exiv2.hpp>
#include <string>

namespace fs = std::filesystem;

// ImageMetadataViewer class scans a directory for image files and extracts EXIF metadata using Exiv2
class ImageMetadataViewer
{
public:
    // Scans the given directory for image files and processes each one
    void scanDirectory(const std::string& path)
    {
        for (const auto & entry : fs::directory_iterator(path))
        {
            if (fs::is_regular_file(entry) && isImageFile(entry.path().extension().string()))
            {
                processFile(entry.path().string());
            }
        }
    }

private:
    // Processes a single file by extracting and displaying its EXIF metadata
    void processFile(const std::string& filepath)
    {
        try
        {
            auto image = Exiv2::ImageFactory::open(filepath);
            if (!image)
            {
                std::cerr << "Error: Could not read " << filepath << "\n";
                return;
            }

            image->readMetadata();
            Exiv2::ExifData& exifData = image->exifData();

            std::cout << "File: " << filepath << "\n";
            for (const auto& entry : exifData)
            {
                std::cout << entry.key() << " = " << entry.value() << "\n";
            }
            std::cout << "---------------------------------------\n";
        }
        catch (const Exiv2::Error& e)
        {
            std::cerr << "Error reading metadata: " << e.what() << "\n";
        }
    }

    // Checks if the file extension matches one of the supported image formats
    bool isImageFile(const std::string& ext)
    {
        return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".tiff" || ext == ".bmp";
    }
};

int main()
{
    std::string directory;
    std::cout << "Enter the directory containing image files: ";
    std::getline(std::cin, directory);

    ImageMetadataViewer viewer;
    viewer.scanDirectory(directory);

    return 0;
}
