#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include "utils.h"

using std::string;
using std::vector;

#define PROGRAM_ARGUMENTS int argc, char** argv
#define SET_PROGRAM_ARGUMENTS argc, argv

#define NULL_OR(first, second) !std::string(first).empty()? first : second  

bool has_argument(PROGRAM_ARGUMENTS, REF(string) searched);
int find_argument(PROGRAM_ARGUMENTS, REF(string) searched);

string get_argument(PROGRAM_ARGUMENTS, int index);
string get_splits_argument(PROGRAM_ARGUMENTS, REF(string) name, char del);
string str_splits(REF(string) str, char del);

vector<string> get_array_splits_args(PROGRAM_ARGUMENTS, REF(string) name, char del);

#endif