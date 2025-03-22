# Task Manager

## Overview
Task Manager is a JSON-based command-line tool built in C++ that allows users to manage a list of tasks. Users can add new tasks, list tasks, mark tasks as completed, and remove tasks. Tasks are stored persistently in a JSON file name tasks.json.

## Features
- Add, list, complete, and remove tasks.
- Persistent storage via a JSON file.
- Demonstrates modern C++ features and clean code practices.

## Build Instructions
Compile with the following command:
```sh
g++ -std=c++17 taskmanager.cpp -o taskmanager
```
## Usage
1. Run the executable:

```sh
./taskmanager
```
2. Follow on-screen prompts to:

- Add new tasks by entering a description.

- List all tasks.

- Mark a task as completed.

- Remove a task.

3. The tool will display your current tasks and update the JSON file accordingly.
