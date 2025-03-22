# Image Metadata Extractor

## Overview
Image Metadata Extractor is a command-line tool that extracts and displays EXIF metadata from image files using Exiv2. It supports common image formats such as `.jpg`, `.jpeg`, `.png`, `.tiff`, and `.bmp`.

## Features
- Scans a directory for image files.
- Extracts and displays EXIF metadata as key-value pairs.

## Build Instructions
```sh
g++ -std=c++17 imagemetadataextractor.cpp -o imagemetadataextractor -lexiv2
```
## Usage
1. Run the viewer:
```sh
./imagemetadataextractor
```
2. Enter the directory containing your image files when prompted.

3. The program outputs EXIF metadata for each image.
