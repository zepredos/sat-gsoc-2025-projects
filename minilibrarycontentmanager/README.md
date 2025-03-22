# Mini Library Content Manager

## Overview
The Mini Library Content Manager is an integrated tool that scans a specified directory, extracts media metadata, and stores the data persistently in an SQLite database. It supports custom tagging of files and provides an interactive CLI for querying and managing the media library. This project combines features from filesystem scanning, metadata extraction, concurrency, and database integration.

## Features
- **Directory Scanning & Real-Time Monitoring:**
  Performs an initial recursive scan and monitors the directory in real time using inotify.
- **Metadata Extraction:**
  Uses TagLib to extract metadata from audio files (e.g., `.mp3`, `.wav`) and simulates metadata for other file types.
- **Concurrent Processing:**
  Utilises multiple worker threads and a thread-safe queue for concurrent metadata extraction.
- **Persistent Storage:**
  Stores metadata in an SQLite database (`library.db`) using an "INSERT OR REPLACE" strategy to prevent duplicates.
- **Custom Tagging:**
  Allows users to add and view custom tags for individual files.
- **Interactive CLI:**
  Provides commands to view in-memory metadata, query the database, add custom tags, and view tags.

## Build Instructions
Compile with:
```sh
g++ -std=c++17 minilibrarycontentmanager.cpp -o minilibrarycontentmanager -lsqlite3 -ltag -pthread
```
## Usage
1. Run the Program:
```sh
./minilibrarycontentmanager
```
2. Enter Directory:
When prompted, input the directory to scan (e.g., /home/usr/Music).

3. Scanning & Processing:
The tool performs a full recursive scan, extracts metadata from media files, and stores the information in the SQLite database.

4. Summary Message:
Once scanning is complete, a summary message is displayed:
```sh
Scan complete. Records have been stored in the database.
Enter 'help' for a list of available commands.
```
5. Interactive CLI Commands:
Use the following commands:

- help – Lists available commands.

- list – Displays in-memory metadata.

- db – Displays the contents of the SQLite database.

- tag <filepath> <tag> – Adds a custom tag to the specified file.

- viewtag <filepath> – Displays custom tags for a specified file.

- exit – Exits the CLI loop.

## Future Enhancements

- Advanced Querying: Implement filters and search capabilities based on metadata and custom tags.

- Enhanced CLI/GUI: Develop a richer interactive interface.

- Extended Metadata Extraction: Add support for extracting metadata from images and videos.

- Integration: Plan for integration with tools such as Ossia Score via a dedicated plugin.
