# Audio Metadata Extractor

## Overview
Audio Metadata Extractor is a CLI tool that uses TagLib to extract metadata from audio files, such as artist, album, title, year, and duration from `.mp3` and `.wav` files.

## Features
- Extracts and displays metadata from audio files.
- Demonstrates integration with TagLib for media processing.

## Build Instructions
```sh
g++ -std=c++17 audiometadataextractor.cpp -o audiometadataextractor -ltag
```
## Usage
Run the extractor:
```sh
./audiometadataextractor
```
2. Enter the directory containing audio files when prompted.

3. The program displays the extracted metadata.
