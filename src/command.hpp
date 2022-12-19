#include <iostream>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "utilities.hpp"

enum CommandCode {
    invalid = 0,
    cd = 1,
};

inline
std::ostream &operator<<(std::ostream &os, CommandCode code) {
    switch (code) {
        case invalid:
            os << "invalid";
            break;
        case cd:
            os << "cd";
            break;
    }

    return os;
}

class Command {
    public:
        Command(std::string input) {
            std::vector<std::string> tokens = split(input, " ");

            // Parse command name into a command code
            code = parse_code(tokens[0]);

            // Remove first element (command name) to obtain vector of args
            tokens.erase(tokens.begin());
            args = tokens;
        }

        CommandCode parse_code(std::string command) {
            std::map<std::string, CommandCode> code_map;

            code_map["cd"] = CommandCode::cd;

            // If we have been given a valid code
            if (code_map.find(command) != code_map.end()) {
                return code_map[command];
            } else {
                return CommandCode::invalid;
            }
        }

        CommandCode get_code() {
            return code;
        }

        std::vector<std::string> get_args() {
            return args;
        }

        bool is_invalid() {
            return code == CommandCode::invalid;
        }

        int execute() {
            switch (code) {
                case invalid:
                    return -1;
                    break;
                case cd:
                    return execute_cd();
                    break;
                default:
                    return -1;
            }
        }

        int execute_cd() {
            if (args.size() == 0 or args[0] == "~") {
                // If the user enters "cd" without any arguments, or "cd ~",
                // they want to go back to their home directory
                std::string home_dir = get_home_dir();

                // Check if this path points to a directory
                if (std::filesystem::is_directory(home_dir)) {
                    // Then we can safely change directories
                    std::filesystem::current_path(home_dir);
                    return 0;
                } else {
                    return -1;
                }
            }

            for (size_t i = 0; i < args.size(); i++) {
                std::string arg = args[i];
                // If this argument is not an option, then it is the path to cd to
                if (arg[0] != '-') {
                    // Check whether this is an absolute or relative path
                    if (arg[0] == '/' || arg[0] == '~') {
                        // This path is absolute
                        std::string new_path = replace_home_dir(arg);
                        // Either successfully change directory and return 0
                        // or unsuccessfully change directory and return -1
                        int status = change_dir(new_path);
                        if (status != 0) {
                            std::cout << new_path << " is not a directory.\n";
                        }
                        return status;
                    } else {
                        // This path is relative, so we need to start at the cwd,
                        // and then move through each step one at a time
                        // For example, if you do cd ../../some_dir,
                        // you can't just paste that to the end of the cwd

                        // Get each of the individual directory moves
                        std::vector<std::string> moves = split(arg, "/");
                        // Loop through each move
                        std::vector<std::string>::iterator move;
                        for (move = moves.begin(); move != moves.end(); move++) {
                            // "." represents the current directory, so don't move
                            if (*move == ".") {
                                continue;
                            }

                            // ".." represents the previous directory, so move up
                            if (*move == "..") {
                                std::string new_path = move_up_dir((std::string)std::filesystem::current_path());
                                int status = change_dir(new_path);
                                if (status != 0) {
                                    std::cout << new_path << " is not a directory.\n";
                                }
                            } else {
                                std::string new_path = (std::string)std::filesystem::current_path() + "/" + (*move);
                                int status = change_dir(new_path);
                                if (status != 0) {
                                    std::cout << new_path << " is not a directory.\n";
                                }
                            }
                        }
                        
                        return 0;
                    }
                }
            }



            return 0;
        }

        int change_dir(std::string path) {
            // Check if this path points to a directory
            if (std::filesystem::is_directory(path)) {
                // Then we can safely change directories
                std::filesystem::current_path(path);
                return 0;
            } else {
                return -1;
            }
        }

        std::string replace_home_dir(std::string path) {
            size_t index = path.find('~');

            if (index != std::string::npos) {
                // Get the rest of the string
                std::string before = path.substr(0, index);
                std::string after = path.substr(index + 1, path.size() - 1);

                // Get the home directory
                std::string home_dir = get_home_dir();

                // Stitch them all together
                return before + home_dir + after;
            } else {
                // If ~ does not appear in the path, there's nothing to replace
                return path;
            }
        }

        std::string get_home_dir() {
            std::string home_dir = getenv("HOME");
            if (home_dir == "") {
                home_dir = getpwuid(getuid())->pw_dir;
            }

            return home_dir;
        }

        std::string move_up_dir(std::string path) {
            // Get each of the individual directories
            std::vector<std::string> dirs = split(path, "/");
            // Remove the last one (the current directory)
            dirs.pop_back();

            // Reconstruct the vector of directories into a path and return it
            return reconstruct_path(dirs);
        }

        std::string reconstruct_path(std::vector<std::string> dirs) {
            std::string path = "";

            std::vector<std::string>::iterator dir;
            for (dir = dirs.begin(); dir != dirs.end(); dir++) {
                path += "/" + (*dir);
            }

            return path;
        }

    private:
        CommandCode code;
        std::vector<std::string> args;
};