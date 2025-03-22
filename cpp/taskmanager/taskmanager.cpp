#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <limits>
#include <string>

using json = nlohmann::json;

// Represents a task with an ID, description, and completion status
struct Task
{
    int id;                      // Unique identifier for the task
    std::string description;     // Description of the task
    bool completed;              // Task completion status
};

// Manages tasks, including adding, listing, completing, and removing them
class TaskManager
{
private:
    std::vector<Task> tasks;             // Container for storing tasks
    const std::string filename = "tasks.json";  // File to persist tasks

    // Loads tasks from a JSON file if it exists
    void loadTasks()
    {
        std::ifstream file(filename);
        if (!file)
        {
            return; // If the file does not exist, return without error
        }

        json j;
        file >> j;
        file.close();

        tasks.clear();
        for (const auto& item : j)
        {
            if (item.contains("id") && item.contains("description") && item.contains("completed"))
            {
                tasks.push_back({ item["id"].get<int>(),
                                  item["description"].get<std::string>(),
                                  item["completed"].get<bool>() });
            }
            else
            {
                std::cerr << "Warning: JSON entry missing required fields. Skipping entry.\n";
            }
        }
    }

    // Saves the current list of tasks to a JSON file
    void saveTasks() const
    {
        json j;
        for (const auto& task : tasks)
        {
            j.push_back({ {"id", task.id}, {"description", task.description}, {"completed", task.completed} });
        }
        std::ofstream file(filename);
        file << j.dump(4);  // Pretty-print JSON with indentation
    }

public:
    // Constructor that initialises the task manager by loading tasks from the file
    TaskManager()
    {
        loadTasks();
    }

    // Adds a new task with a given description
    void addTask(const std::string& description)
    {
        int id = tasks.empty() ? 1 : tasks.back().id + 1;
        tasks.push_back({ id, description, false });
        saveTasks();
        std::cout << "Task added successfully.\n";
    }

    // Lists all tasks with their status
    void listTasks() const
    {
        if (tasks.empty())
        {
            std::cout << "No tasks available.\n";
            return;
        }

        for (const auto& task : tasks)
        {
            std::cout << "[" << (task.completed ? "x" : " ") << "] " << task.id
                      << ": " << task.description << "\n";
        }
    }

    // Marks a task as completed
    void completeTask(int id)
    {
        auto it = std::find_if(tasks.begin(), tasks.end(), [id](const Task& task)
        {
            return task.id == id;
        });

        if (it != tasks.end())
        {
            it->completed = true;
            saveTasks();
            std::cout << "Task marked as completed.\n";
        }
        else
        {
            std::cout << "Task not found.\n";
        }
    }

    // Removes a task by its ID
    void removeTask(int id)
    {
        auto new_end = std::remove_if(tasks.begin(), tasks.end(), [id](const Task& task)
        {
            return task.id == id;
        });

        if (new_end != tasks.end())
        {
            tasks.erase(new_end, tasks.end());
            saveTasks();
            std::cout << "Task removed successfully.\n";
        }
        else
        {
            std::cout << "Task not found.\n";
        }
    }
};

// Handling the user interface for the Task Manager
int main()
{
    TaskManager tm;
    int choice{};

    while (true)
    {
        std::cout << "\nTask Manager:\n";
        std::cout << "1. Add Task\n";
        std::cout << "2. List Tasks\n";
        std::cout << "3. Complete Task\n";
        std::cout << "4. Remove Task\n";
        std::cout << "5. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice)
        {
            case 1:
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear newline from previous input
                std::string desc;
                std::cout << "Enter task description: ";
                std::getline(std::cin, desc);
                tm.addTask(desc);
                break;
            }
            case 2:
                tm.listTasks();
                break;
            case 3:
            {
                int id;
                std::cout << "Enter task ID to complete: ";
                std::cin >> id;
                tm.completeTask(id);
                break;
            }
            case 4:
            {
                int id;
                std::cout << "Enter task ID to remove: ";
                std::cin >> id;
                tm.removeTask(id);
                break;
            }
            case 5:
                std::cout << "Exiting Task Manager.\n";
                return 0;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
}
