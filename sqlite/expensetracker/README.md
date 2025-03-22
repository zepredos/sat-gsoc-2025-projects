# Expense Tracker

## Overview
Expense Tracker is a CLI application that lets users log daily expenses. It stores expense data in an SQLite database and can generate simple expense reports.

## Features
- Log expenses with date, category, amount, and description.
- Persistent storage using SQLite.
- Generate and display expense reports.

## Build Instructions
Compile with:
```sh
g++ -std=c++17 expensetracker.cpp -o expensetracker -lsqlite3
```
## Usage
1. Run the expense tracker:
```sh
./expensetracker
```
2. Follow the prompts to add an expense, view all expenses, or remove an entry.

3. Data is saved in an SQLite database.
