# File Organiser

## Overview
File Organiser is a command-line tool that scans a specified directory and moves files into categorised folders based on their file extensions (e.g., Images, Documents, Videos, Audio, Archives).

## Features
- Scans a given directory for files.
- Moves files into pre-defined folders based on their extensions.
- Uses a mapping from file extensions to category names.

## Build Instructions
Compile with:
```sh
g++ -std=c++17 fileorganiser.cpp -o fileorganiser
```
## Usage
1. Run the program:
```sh
./fileorganiser
```
2. When prompted, enter the directory path to organize (e.g., /home/zep/Documents).

3. The tool moves files into corresponding subdirectories based on their extensions.

4. Check the specified directory for newly created folders containing the organized files.
