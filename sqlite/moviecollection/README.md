# Movie Collection

## Overview
Movie Collection is a CLI tool for managing a personal movie database. It allows users to add, view, and remove movie records. The data is stored persistently in an SQLite database.

## Features
- Add, view, and remove movie records.
- Persistent storage using SQLite.
- Simple, interactive menu for managing movies.

## Build Instructions
```sh
g++ -std=c++17 moviecollection.cpp -o moviecollection -lsqlite3
```
## Usage
1. Run the program:
```sh
./moviecollection
```
2. Follow the on-screen menu to add a new movie, display the collection, or remove a movie.

3. Movie data is stored in an SQLite database.
