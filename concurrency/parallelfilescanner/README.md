# Parallel File Scanner

## Overview
Parallel File Scanner demonstrates the use of concurrency in C++ by scanning a directory using multiple threads. It uses a custom thread-safe queue to distribute file paths among threads, illustrating parallel processing.

## Features
- Concurrent scanning of a directory using multiple threads.
- Uses a thread-safe queue for inter-thread communication.
- Efficiently lists file paths from the specified directory.

## Build Instructions
```sh
g++ -std=c++17 parallelfilescanner.cpp -o parallelfilescanner -pthread
```
## Usage
1. Run the scanner:
```sh
./parallel_file_scanner
```
2. Enter the directory to scan when prompted.

3. The tool outputs file paths as they are discovered.
