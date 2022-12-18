#include <iostream>
#include <vector>
#include <filesystem>

// --------------------
// Function prototypes
int shell_loop();
std::string get_current_dir_name();
std::vector<std::string> split(std::string, std::string);
// --------------------

int main() {
    // Print boilerplate information
    std::cout << "dysh version 0.1\n";
    std::cout << "Developed by Dylan Tuttle\n";

    // Display current working directory
    std::cout << "\nCurrent Working Directory:\n";
    std::cout << (std::string) std::filesystem::current_path() << "\n\n";

    // Enter the shell loop
    shell_loop();

    return 0;
}

// Function for the main loop of the shell
// this component will read input and pass it along to the command parser
int shell_loop() {
    std::cout << "Enter commands to run them or \"exit\" to quit the shell\n";

    bool exit = false;
    while (!exit) {
        // Print the shell input indicator (name-of-cwd>)
        std::cout << '\n' << get_current_dir_name() << "> ";

        // Get input
        std::string input;
        std::getline(std::cin, input);
        std::cout << input << '\n';

        // Do something with the input

        // If the user has chosen to exit the shell, break the loop
        if (input == "exit") {
            exit = true;
        }
    }

    return 0;
}

// Function to get the name of the current working directory,
// i.e. the part of the cwd that occurs after the final "/"
std::string get_current_dir_name() {
    std::string cwd = std::filesystem::current_path();
    std::string delimiter = "/";

    std::vector<std::string> tokens = split(cwd, delimiter);

    std::string dir_name = tokens.back();
    return dir_name;
}

// Generic helper function to split a string by a given string delimiter
std::vector<std::string> split(std::string str, std::string delim) {
    std::vector<std::string> tokens;
    size_t startIndex = 0;
    for (size_t found = str.find(delim); found != std::string::npos; found = str.find(delim, startIndex))
    {
        tokens.emplace_back(str.begin()+startIndex, str.begin()+found);
        startIndex = found + delim.size();
    }

    if (startIndex != str.size())
        tokens.emplace_back(str.begin()+startIndex, str.end());

    return tokens;  
}