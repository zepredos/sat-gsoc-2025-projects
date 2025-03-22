# Multithreaded Metadata Extractor

## Overview
Multithreaded Metadata Extractor extracts media metadata concurrently using multiple worker threads. It demonstrates a producer–consumer model with a thread-safe queue and synchronisation mechanisms.

## Features
- Concurrent metadata extraction from media files.
- Uses multiple worker threads to process file paths from a thread-safe queue.
- Stores extracted metadata in an in-memory map.

## Build Instructions
```sh
g++ -std=c++17 multithreadedmetadataextractor.cpp -o multithreadedmetadataextractor -pthread -ltag
```
## Usage
1. Run the extractor:
```sh
./metadata_extractor
```
2. Enter the directory to scan when prompted.

3. The tool extracts metadata (e.g., from audio files) and displays it on the console.
