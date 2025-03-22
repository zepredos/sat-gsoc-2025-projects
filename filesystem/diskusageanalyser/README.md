# Disk Usage Analyser

## Overview
Disk Usage Analyser is a CLI tool that recursively scans directories, calculates file sizes, and identifies the largest files. It displays the results in a neatly formatted table, helping users understand disk space usage.

## Features
- Recursively scans directories.
- Calculates and sorts file sizes.
- Displays results in a formatted table.

## Build Instructions
Compile with:
```sh
g++ -std=c++17 diskusageanalyser.cpp -o diskusageanalyser
```
## Usage
1. Run the tool:
```sh
./diskusageanalyser
```
2. Enter the directory you want to analyse when prompted.

3. The tool displays a table with file paths and their corresponding sizes.
