#include <iostream>
#include <iomanip>
#include <string>
#include <sqlite3.h>

// Represents a movie with title, director, release year, and rating
struct Movie
{
    int id;                 // Unique identifier for the movie
    std::string title;      // Title of the movie
    std::string director;   // Director of the movie
    int year;               // Release year
    double rating;          // Movie rating
};

// Manages a collection of movies stored in an SQLite database
class MovieCollection
{
private:
    sqlite3* db;  // SQLite database pointer

    // Executes an SQL query and prints an error message if it fails
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
    // Constructor: Opens the database and creates the movies table if it doesn't exist
    MovieCollection()
    {
        if (sqlite3_open("movies.db", &db) != SQLITE_OK)
        {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        }
        executeQuery("CREATE TABLE IF NOT EXISTS movies (id INTEGER PRIMARY KEY, title TEXT, director TEXT, year INTEGER, rating REAL);");
    }

    // Destructor: Closes the SQLite database
    ~MovieCollection()
    {
        sqlite3_close(db);
    }

    // Adds a new movie to the collection by prompting the user for input
    void addMovie()
    {
        std::string title, director;
        int year;
        double rating;

        std::cout << "Enter movie title: ";
        std::getline(std::cin, title);
        std::cout << "Enter director: ";
        std::getline(std::cin, director);
        std::cout << "Enter release year: ";
        std::cin >> year;
        std::cout << "Enter rating: ";
        std::cin >> rating;
        std::cin.ignore();  // Clear newline left in the input buffer

        std::string query = "INSERT INTO movies (title, director, year, rating) VALUES ('"
                            + title + "', '" + director + "', " + std::to_string(year)
                            + ", " + std::to_string(rating) + ");";
        executeQuery(query);
    }

    // Removes a movie from the collection based on the movie's ID
    void removeMovie()
    {
        int id;
        std::cout << "Enter movie ID to remove: ";
        std::cin >> id;
        std::cin.ignore();  // Clear the input buffer

        std::string query = "DELETE FROM movies WHERE id = " + std::to_string(id) + ";";
        executeQuery(query);
    }

    // Retrieves and displays all movies in a formatted table
    void viewMovies()
    {
        std::string query = "SELECT * FROM movies;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            // Define column widths for table formatting
            constexpr int idWidth = 6;
            constexpr int titleWidth = 25;
            constexpr int directorWidth = 20;
            constexpr int yearWidth = 6;
            constexpr int ratingWidth = 8;
            // Total width accounts for all columns and additional formatting characters
            const int totalWidth = idWidth + titleWidth + directorWidth + yearWidth + ratingWidth + 16;

            std::cout << std::string(totalWidth, '-') << std::endl;
            std::cout << "| " << std::setw(idWidth) << "ID" << " | "
                      << std::setw(titleWidth) << "Title" << " | "
                      << std::setw(directorWidth) << "Director" << " | "
                      << std::setw(yearWidth) << "Year" << " | "
                      << std::setw(ratingWidth) << "Rating" << " |" << std::endl;
            std::cout << std::string(totalWidth, '-') << std::endl;

            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int id = sqlite3_column_int(stmt, 0);
                std::string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string director = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                int year = sqlite3_column_int(stmt, 3);
                double rating = sqlite3_column_double(stmt, 4);

                std::cout << "| " << std::setw(idWidth) << id << " | "
                          << std::setw(titleWidth) << title << " | "
                          << std::setw(directorWidth) << director << " | "
                          << std::setw(yearWidth) << year << " | "
                          << std::setw(ratingWidth) << std::fixed << std::setprecision(1) << rating << " |" << std::endl;
            }
            std::cout << std::string(totalWidth, '-') << std::endl;
        }
        sqlite3_finalize(stmt);
    }
};

int main()
{
    MovieCollection collection;
    int choice{};

    while (true)
    {
        std::cout << "\nMovie Collection Manager\n";
        std::cout << "1. Add Movie\n";
        std::cout << "2. View Movies\n";
        std::cout << "3. Remove Movie\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        std::cin.ignore();  // Clear buffer after integer input

        switch (choice)
        {
            case 1:
                collection.addMovie();
                break;
            case 2:
                collection.viewMovies();
                break;
            case 3:
                collection.removeMovie();
                break;
            case 4:
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}
