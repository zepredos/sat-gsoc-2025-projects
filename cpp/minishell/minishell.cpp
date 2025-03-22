#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>

// Splits the input string into tokens based on whitespace
// Returns a vector of tokens
std::vector<std::string> tokenise(const std::string& input)
{
    std::istringstream stream(input);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Executes the built-in 'cd' command
// Expects tokens[1] to be the target directory
void changeDirectory(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        std::cerr << "Usage: cd <directory>\n";
        return;
    }

    if (chdir(tokens[1].c_str()) != 0)
    {
        perror("chdir failed");
    }
}

// Executes an external command by forking and using execvp
// Converts the vector of strings into a format acceptable by execvp
void executeCommand(const std::vector<std::string>& args)
{
    if (args.empty())
    {
        return;
    }

    std::vector<char*> argv;
    for (const auto& arg : args)
    {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process executes the command
        if (execvp(argv[0], argv.data()) == -1)
        {
            perror("execvp failed");
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process waits for the child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}

// Main shell loop: reads user input, tokenises it, and processes commands
void shellLoop()
{
    std::string input;

    while (true)
    {
        std::cout << "mini-shell> ";

        if (!std::getline(std::cin, input) || input == "exit")
        {
            break;
        }

        auto tokens = tokenise(input);
        if (tokens.empty())
        {
            continue;
        }

        if (tokens[0] == "cd")
        {
            changeDirectory(tokens);
        }
        else
        {
            executeCommand(tokens);
        }
    }
}

int main()
{
    shellLoop();
    return 0;
}
