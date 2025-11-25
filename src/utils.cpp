#include "utils/string_utils.h"
#include <string>
#include <cstring>

bool has_argument(PROGRAM_ARGUMENTS, REF(string) searched)
{
    return find_argument(SET_PROGRAM_ARGUMENTS, searched) != -1;
}

int find_argument(PROGRAM_ARGUMENTS, REF(string) searched)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], searched.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

string get_argument(PROGRAM_ARGUMENTS, int index)
{
    if (index >= 0 && index < argc) {
        return argv[index];
    }
    return "";
}

string get_splits_argument(PROGRAM_ARGUMENTS, REF(string) name, char del)
{
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        size_t pos = arg.find(del);
        if (pos != std::string::npos) {
            std::string arg_name = arg.substr(0, pos);
            if (arg_name == name) {
                return arg.substr(pos + 1);
            }
        }
    }
    return "";
}

std::string str_splits(REF(string) str, char del)
{
    size_t pos = str.find(del);
    if (pos == std::string::npos) {
        return "";
    }
    return str.substr(pos + 1);
}

vector<string> get_array_splits_args(PROGRAM_ARGUMENTS, REF(string) name, char del)
{
    std::vector<string> result;

    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        size_t pos = arg.find(del);
        if (pos != std::string::npos) {
            std::string arg_name = arg.substr(0, pos);
            if (arg_name == name) {
                std::string value = arg.substr(pos + 1);
                if (!value.empty()) {
                    result.push_back(value);
                }
            }
        }
    }

    return result;
}