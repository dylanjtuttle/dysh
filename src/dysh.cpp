#include <iostream>
#include <vector>
#include <filesystem>

#include "../include/peelo/prompt.hpp"

#include "utilities.hpp"
#include "command.hpp"

// --------------------
// Function prototypes
int shell_loop();
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
    peelo::prompt prompt;

    std::cout << "Enter commands to run them or \"exit\" to quit the shell\n";

    // Enter shell loop
    while (true) {
        // Set the shell input indicator (name-of-cwd> )
        std::string input_indicator = get_current_dir_name() + "> ";
        auto line = prompt.input(input_indicator);
        const auto value = line.value();

        // Skip empty lines.
        if (value.empty()) {
            continue;
        }

        // Do something with the string.
        if (value[0] != '/') {
            prompt.add_to_history(value);

            // If the user has indicated they want to exit the shell, break out of the loop
            if (value == "exit") {
                break;
            }

            // Otherwise, we'll parse this input into a command
            Command command = Command(value);

            // If our new command is not a valid one, ignore it and loop again
            if (command.is_invalid()) {
                std::cout << "Unrecognized command: " << split(value, " ")[0] << '\n';
                continue;
            }

            // Otherwise we have a valid command, so we will execute it
            int status = command.execute();
            if (status != 0) {
                std::cout << "Unable to execute command.\n";
            }
        }

        // The "/historylen" command will change the history len.
        else if (!value.compare(0, 11, "/historylen")) {
            prompt.set_history_max_size(std::atoi(value.c_str() + 11));
        }
        else if (value[0] == '/') {
            std::cout << "Unrecognized command: " << value << '\n';
        }
    }

    return 0;
}