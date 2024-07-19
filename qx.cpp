/*
-------------------------- License - GPLv3 ------------------------------
QX - Quick Execution
Copyright (C) 2024 Citizen7751

<https://github.com/Citizen7751/QX>

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version. This program is distributed in the hope that
it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.

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

    #include <windows.h>
    #define EXT ".exe"

    void enable_ansi_escseq() {
        HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dw_mode = 0;
        GetConsoleMode(h_out, &dw_mode);
        dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(h_out, dw_mode);
    }

    void disable_ansi_escseq() {
        HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dw_mode = 0;
        GetConsoleMode(h_out, &dw_mode);
        dw_mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(h_out, dw_mode);
    }

    #define ANSI_SEQ_ON enable_ansi_escseq()
    #define ANSI_SEQ_OFF disable_ansi_escseq()

#elif __linux__

    #define EXT ""
    #define ANSI_SEQ_ON ;
    #define ANSI_SEQ_OFF ;

#endif

#define RED         "\033[38;2;255;0;0m"
#define GREEN       "\033[38;2;0;255;0m"
#define WHITE       "\033[38;2;255;255;255m"
#define GREY        "\033[38;2;100;100;100m"
#define DEF_COLOR   "\033[0m"

typedef std::string str;


// -------------------------------------------------------
// ------------------ Configurable Part ------------------
// default C compiler & flags used to create the binaries
const str def_qx_cc = "gcc";
const str def_qx_ofile_flag = "-o";
const str def_qx_make_flags = "-s -O3";
// -------------------------------------------------------
// -------------------------------------------------------


inline void print_copyright_notice() {
    std::cout <<
        GREY
        "QX Copyright (C) 2024 Citizen7751\nThis program comes with ABSOLUTELY NO WARRANTY.\n"
        DEF_COLOR;
}

str sanitize_str(const str& input) {
    str cleanstr;
    for (auto c : input) {
        if (c == '\\' || c == '\"')
          cleanstr += '\\';
        cleanstr += c;
    }
    return cleanstr;
}

namespace file_handling {

    bool check_file(const str& fname) {
        std::ifstream f(fname);
        bool success = f.good();
        f.close();
        return success;
    }

    void creation_error(const str& fname) {
        std::cout << RED "[Could not create " DEF_COLOR << fname << RED "]" DEF_COLOR "\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }

    inline void file_already_exists(const str& fname) {
        std::cout << RED "[" DEF_COLOR << fname << RED " already exists] " GREY "Try again." DEF_COLOR "\n";
    }
}

namespace create_file {

    void src(const str& qxfile_src, const str& qxfile_make_command, std::vector<str>& exe_commands) {

        std::ofstream cmdfile(qxfile_src);
        if (!cmdfile.is_open()) file_handling::creation_error(qxfile_src);
        
        cmdfile << "// ***** To recompile this ******\n//" << qxfile_make_command << "\n\n";
        cmdfile << "#include <stdio.h>\n#include <stdlib.h>\n\n";
        cmdfile << "void exe_error(const char* command, const int line) {\n";
        cmdfile << "\tprintf(\"" RED "Error at line [" DEF_COLOR "%d" RED "]: " DEF_COLOR
            "\\\"%s\\\"" RED " Aborting." DEF_COLOR "\\n\", line, command);\n";
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
        cmdfile << "\tprintf(\"" GREY "\\n[Process finished - press Enter to exit]" DEF_COLOR "\");\n";
        cmdfile << "\twhile(getchar()!=\'\\n\');\n\treturn 0;\n}";
        cmdfile.close();
    }

    int exe(const str& make_command) {
        return system(make_command.c_str());
    }
}

namespace user_prompt {

    void get_qx_file_name(str& qxfile_name, str& qxfile_src, str& qxfile_exe, str& qxfile_make_command) {
        do {
            std::cout << WHITE "\nExecutable name: " DEF_COLOR;
            std::getline(std::cin, qxfile_name);

            qxfile_src = qxfile_name + ".c";
            qxfile_exe = qxfile_name + EXT;
            qxfile_make_command = def_qx_cc + " " + def_qx_ofile_flag + " " + qxfile_exe + " " + qxfile_src + " " + def_qx_make_flags;

            if (file_handling::check_file(qxfile_src)) file_handling::file_already_exists(qxfile_src);
            if (file_handling::check_file(qxfile_exe)) file_handling::file_already_exists(qxfile_exe);

        } while (file_handling::check_file(qxfile_src) || file_handling::check_file(qxfile_exe));
    }

    void get_commands(std::vector<str>& commands) {
        std::cout << WHITE "Commands:\n" DEF_COLOR;
        str buff;
        unsigned int line = 1;
        do {
            std::cout << '[' << line << "]> ";
            std::getline(std::cin, buff);
            if (buff != "") commands.push_back(buff);
            line++;
        } while (buff != "");
    }
}


int main(void) {
    ANSI_SEQ_ON;
    print_copyright_notice();

    str qxfile_name, qxfile_src, qxfile_exe, qxfile_make_command;
    std::vector<str> exe_commands;

    user_prompt::get_qx_file_name(qxfile_name, qxfile_src, qxfile_exe, qxfile_make_command);
    user_prompt::get_commands(exe_commands);

    create_file::src(qxfile_src, qxfile_make_command, exe_commands);
    if (!file_handling::check_file(qxfile_src)) file_handling::creation_error(qxfile_src);

    if (create_file::exe(qxfile_make_command)) file_handling::creation_error(qxfile_exe);
    if (!file_handling::check_file(qxfile_exe)) file_handling::creation_error(qxfile_exe);

    std::cout << GREEN "\n[" DEF_COLOR << qxfile_src
              << GREEN " and " DEF_COLOR << qxfile_exe
              << GREEN " are successfully created]" GREY
              "\n[Press Enter to exit]" DEF_COLOR "\n";

    std::cin.get();
    ANSI_SEQ_OFF;
    return EXIT_SUCCESS;
}
