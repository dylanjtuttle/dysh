#include "utilities.hpp"

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