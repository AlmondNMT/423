#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <search term>" << std::endl;
        return 1;
    }

    std::regex pattern(argv[1], std::regex_constants::icase);
    std::filesystem::path searchPath("./"); // Change the path as per your requirement

    for (const auto& file : std::filesystem::recursive_directory_iterator(searchPath)) {
        if (std::filesystem::is_regular_file(file)) {
            std::ifstream infile(file.path());
            std::string line;
            while (std::getline(infile, line)) {
                if (std::regex_search(line, pattern)) {
                    std::cout << "Found " << file.path() << " : " /* << line*/ << std::endl;
                }
            }
        }
    }

    return 0;
}
