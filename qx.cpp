/*
-------------------------- License - GPLv3 ------------------------------
qx - Quick Execution
Copyright (C) 2024 Citizen7751 <https://github.com/Citizen7751>

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or
(at your option) any later version. This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------
*/


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>

#ifdef _WIN32
    #define EXT ".exe"
#elif __linux__
    #define EXT ""
#endif

#define RED             "\033[0;91m"
#define GREEN           "\033[0;92m"
#define DEF_COLOR       "\033[0m"

typedef std::string str;


// -------------------------------------------------------
// -------------------- Editable Part --------------------
// default C compiler & flags used to create the binaries
// feel free to change them
const str def_cc = "gcc";
const str ofile_flag = "-o";
const str make_flags = "-s -O3";
// -------------------------------------------------------
// -------------------------------------------------------


void get_commands(std::vector<str>& commands) {
    str buff;
    unsigned int line = 1;
    do {
        std::cout << '[' << line << "]> ";
        std::getline(std::cin, buff);
        if (buff != "") commands.push_back(buff);
        line++;
    } while (buff != "");
}

str sanitize_str(str& line) {
    str cleanstr;
    for (auto c : line) {
        if (c == '\\' || c == '\"')
          cleanstr += '\\';
        cleanstr += c;
    }
    return cleanstr;
}

bool check_file(str& fname) {
    std::ifstream f(fname);
    bool success = f.is_open();
    f.close();
    return success;
}

void handle_error(str& fname) {
    std::cout << RED "\n-> Could not create [" DEF_COLOR << fname << RED "]!" DEF_COLOR "\n";
    std::cin.get();
    exit(EXIT_FAILURE);
}

void create_src(str& qxfile_src, str& qxfile_make_command, std::vector<str>& exe_commands) {
    
    std::ofstream cmdfile(qxfile_src);
    if (!cmdfile.is_open()) handle_error(qxfile_src);
    
    cmdfile << "// ***** To recompile this ******\n//" << qxfile_make_command << "\n\n";
    cmdfile << "#include <stdio.h>\n#include <stdlib.h>\n\n";
    cmdfile << "void exe_error(const char* command, const int line) {\n";
    cmdfile << "\tprintf(\"\033[0;91mError at line [\033[0m%d\033[0;91m]: \033[0m\\\"%s\\\"\033[0;91m Aborting.\033[0m\\n\", line, command);\n";
    cmdfile << "\twhile (getchar()!='\\n');\n\texit(1);\n}\n\n";
    cmdfile << "int main(void) {\n\n\n";
    cmdfile << "\tconst char* commands[] ={\n";
    
    for (auto line : exe_commands) {
        line = sanitize_str(line);
        cmdfile << "\t\t\"" << line << "\",\n";
    }

    cmdfile << "\t};\n\n\n";
    cmdfile << "\tconst int length = sizeof(commands)/sizeof(commands[0]);\n";
    cmdfile << "\tfor (unsigned int i=0; i<length; i++)\n";
    cmdfile << "\t\tif (system(commands[i])) exe_error(commands[i], i+1);\n\n";
    cmdfile << "\tprintf(\"\033[0;92mFinished.\033[0m\");\n";
    cmdfile << "\twhile(getchar()!=\'\\n\');\n\treturn 0;\n}";
    cmdfile.close();
}

int create_exe(str& make_command) {
    return system(make_command.c_str());
}

int main(void) {
    str qxfile_name, qxfile_src, qxfile_exe, qxfile_make_command;
    std::vector<str> exe_commands;

    std::cout << "Executable Name: ";   std::getline(std::cin, qxfile_name);
    std::cout << "Commands:\n";         get_commands(exe_commands);

    qxfile_src = qxfile_name + ".c";
    qxfile_exe = qxfile_name + EXT;
    qxfile_make_command = def_cc + " " + ofile_flag + " " + qxfile_exe + " " + qxfile_src + " " + make_flags;

    create_src(qxfile_src, qxfile_make_command, exe_commands);
    if (!check_file(qxfile_src)) handle_error(qxfile_src);

    if (create_exe(qxfile_make_command)) handle_error(qxfile_exe);
    if (!check_file(qxfile_exe)) handle_error(qxfile_exe);

    std::cout << GREEN "\n-> [" DEF_COLOR << qxfile_src
              << GREEN "] and [" DEF_COLOR << qxfile_exe
              << GREEN "] are successfully created." DEF_COLOR
              "\n\n<Press Enter to exit.>\n";
    std::cin.get();
    return EXIT_SUCCESS;
}