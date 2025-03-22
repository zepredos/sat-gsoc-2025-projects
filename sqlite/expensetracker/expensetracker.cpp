#include <iostream>
#include <iomanip>
#include <string>
#include <sqlite3.h>

// Represents an expense entry
struct Expense
{
    int id;                      // Unique identifier for the expense
    std::string date;            // Date of the expense
    std::string category;        // Expense category
    double amount;               // Expense amount
    std::string description;     // Description of the expense
};

// Manages expenses by interacting with an SQLite database
class ExpenseTracker
{
private:
    sqlite3* db;  // SQLite database pointer

    // Executes an SQL query and prints an error if it fails
    void executeQuery(const std::string& query)
    {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

public:
    // Constructor: opens the database and creates the expenses table if it doesn't exist
    ExpenseTracker()
    {
        if (sqlite3_open("expenses.db", &db) != SQLITE_OK)
        {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        }
        executeQuery("CREATE TABLE IF NOT EXISTS expenses (id INTEGER PRIMARY KEY, date TEXT, category TEXT, amount REAL, description TEXT);");
    }

    // Destructor: closes the database
    ~ExpenseTracker()
    {
        sqlite3_close(db);
    }

    // Adds a new expense entry to the database
    void addExpense(const Expense& expense)
    {
        std::string query = "INSERT INTO expenses (date, category, amount, description) VALUES ('" +
                            expense.date + "', '" + expense.category + "', " + std::to_string(expense.amount) +
                            ", '" + expense.description + "');";
        executeQuery(query);
    }

    // Retrieves and displays all expenses from the database in a formatted table
    void viewExpenses()
    {
        std::string query = "SELECT * FROM expenses;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            // Define column widths for formatting
            constexpr int idWidth = 6;
            constexpr int dateWidth = 12;
            constexpr int categoryWidth = 12;
            constexpr int amountWidth = 8;
            constexpr int descriptionWidth = 20;
            // Total width accounts for columns plus separators and spaces
            const int totalWidth = idWidth + dateWidth + categoryWidth + amountWidth + descriptionWidth + 16;

            std::cout << std::string(totalWidth, '-') << std::endl;
            std::cout << "| " << std::setw(idWidth) << "ID" << " | "
                      << std::setw(dateWidth) << "Date" << " | "
                      << std::setw(categoryWidth) << "Category" << " | "
                      << std::setw(amountWidth) << "Amount" << " | "
                      << std::setw(descriptionWidth) << "Description" << " |" << std::endl;
            std::cout << std::string(totalWidth, '-') << std::endl;

            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int id = sqlite3_column_int(stmt, 0);
                std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                double amount = sqlite3_column_double(stmt, 3);
                std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

                std::cout << "| " << std::setw(idWidth) << id << " | "
                          << std::setw(dateWidth) << date << " | "
                          << std::setw(categoryWidth) << category << " | "
                          << std::setw(amountWidth) << std::fixed << std::setprecision(2) << amount << " | "
                          << std::setw(descriptionWidth) << description << " |" << std::endl;
            }
            std::cout << std::string(totalWidth, '-') << std::endl;
        }
        sqlite3_finalize(stmt);
    }
};

int main()
{
    ExpenseTracker tracker;
    tracker.addExpense({ 0, "2025-03-11", "Food", 15.75, "Lunch" });
    tracker.addExpense({ 0, "2025-03-11", "Transport", 10.50, "Bus fare" });
    tracker.viewExpenses();
    return 0;
}

