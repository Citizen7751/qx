/*
-------------------------- License - GPLv3 ------------------------------
QX - Quick Execution
Copyright (C) 2024 Citizen7751

<https://github.com/Citizen7751/qx>

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

    //enabling virtual terminal processing to use
    //the ANSI esc sequences to color the command prompt on windows

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

namespace Colors {
    const char* RED = "\033[38;2;255;0;0m";
    const char* GREEN = "\033[38;2;0;255;0m";
    const char* WHITE = "\033[38;2;255;255;255m";
    const char* GREY = "\033[38;2;100;100;100m";
    const char* DEF_COLOR = "\033[0m";
}


// -------------------------------------------------------
// ------------------ Configurable Part ------------------
// default C compiler & flags used to create the binaries
const std::string def_qx_cc = "gcc";
const std::string def_qx_ofile_flag = "-o";
const std::string def_qx_make_flags = "-s -O3";
// -------------------------------------------------------
// -------------------------------------------------------


inline void print_copyright_notice() {
    std::cout
        << Colors::GREY
        << "QX Copyright (C) 2024 Citizen7751\nThis program comes with ABSOLUTELY NO WARRANTY.\n"
        << Colors::DEF_COLOR;
}

std::string sanitize_str(const std::string& input) {
    std::string cleanstr;
    for (auto c : input) {
        if (c == '\\' || c == '\"')
          cleanstr += '\\';
        cleanstr += c;
    }
    return cleanstr;
}

namespace file_handling {

    bool check_file(const std::string& fname) {
        std::ifstream f(fname);
        bool success = f.good();
        f.close();
        return success;
    }

    void creation_error(const std::string& fname) {
        std::cout
            << Colors::RED << "[Could not create "
            << Colors::DEF_COLOR
            << fname
            << Colors::RED << "]"
            << Colors::DEF_COLOR  << "\n";
        std::cin.get();
        exit(EXIT_FAILURE);
    }

    inline void file_already_exists(const std::string& fname) {
        std::cout
            << Colors::RED << "[" << Colors::DEF_COLOR
            << fname
            << Colors::RED << " already exists] "
            << Colors::GREY << "Try again." << Colors::DEF_COLOR << "\n";
    }
}

namespace create_file {

    void src(
        const std::string& qxfile_src,
        const std::string& qxfile_make_command,
        std::vector<std::string>& exe_commands)
    {
        std::ofstream cmdfile(qxfile_src);
        if (!cmdfile.is_open()) file_handling::creation_error(qxfile_src);
        
        cmdfile << "// ***** To recompile this ******\n// "
                << qxfile_make_command << "\n\n"
                << "#include <stdio.h>\n#include <stdlib.h>\n\n"
                << "void exe_error(const char* command, const int line) {\n"
                << "\tprintf(\"" << Colors::RED << "Error at line ["
                << Colors::DEF_COLOR << "%d"<< Colors::RED << "]: " << Colors::DEF_COLOR
                << "\\\"%s\\\"" << Colors::RED << " Aborting."
                << Colors::DEF_COLOR << "\\n\", line, command);\n"
                << "\twhile (getchar()!='\\n');\n\texit(1);\n}\n\n"
                << "int main(void) {\n\n\n"
                << "\tconst char* commands[] = {\n";
        
        for (auto line : exe_commands) {
            line = sanitize_str(line);
            cmdfile << "\t\t\"" << line << "\",\n";
        }

        cmdfile << "\t};\n\n\n\tconst int length = sizeof(commands)/sizeof(commands[0]);\n"
                << "\tfor (unsigned int i=0; i<length; i++)\n"
                << "\t\tif (system(commands[i])) exe_error(commands[i], i+1);\n\n"
                << "\tprintf(\"" << Colors::GREY << "\\n[Process finished - press Enter to exit]"
                << Colors::DEF_COLOR << "\");\n"
                << "\twhile(getchar()!=\'\\n\');\n\treturn 0;\n}";
        cmdfile.close();
    }

    int exe(const std::string& make_command) {
        return system(make_command.c_str());
    }
}

namespace user_prompt {

    void get_qx_file_name(
        std::string& qxfile_name,
        std::string& qxfile_src,
        std::string& qxfile_exe,
        std::string& qxfile_make_command)
    {
        do {
            std::cout << Colors::WHITE << "\nExecutable name: " << Colors::DEF_COLOR;
            std::getline(std::cin, qxfile_name);

            qxfile_src = qxfile_name + ".c";
            qxfile_exe = qxfile_name + EXT;
            qxfile_make_command = def_qx_cc + " " + def_qx_ofile_flag + " " + qxfile_exe + " " + qxfile_src + " " + def_qx_make_flags;

            if (file_handling::check_file(qxfile_src)) file_handling::file_already_exists(qxfile_src);
            if (file_handling::check_file(qxfile_exe)) file_handling::file_already_exists(qxfile_exe);

        } while (file_handling::check_file(qxfile_src) || file_handling::check_file(qxfile_exe));
    }

    void get_commands(std::vector<std::string>& commands) {
        std::cout << Colors::WHITE << "Commands:\n" << Colors::DEF_COLOR;
        std::string buff;
        unsigned int line = 1;
        do {
            std::cout << '[' << line++ << "]> ";
            std::getline(std::cin, buff);
            if (buff != "") commands.push_back(buff);
        } while (buff != "");
    }
}


int main(void) {
    ANSI_SEQ_ON;
    print_copyright_notice();

    std::string qxfile_name, qxfile_src, qxfile_exe, qxfile_make_command;
    std::vector<std::string> exe_commands;

    user_prompt::get_qx_file_name(qxfile_name, qxfile_src, qxfile_exe, qxfile_make_command);
    user_prompt::get_commands(exe_commands);

    create_file::src(qxfile_src, qxfile_make_command, exe_commands);
    if (!file_handling::check_file(qxfile_src)) file_handling::creation_error(qxfile_src);

    if (create_file::exe(qxfile_make_command)) file_handling::creation_error(qxfile_exe);
    if (!file_handling::check_file(qxfile_exe)) file_handling::creation_error(qxfile_exe);

    std::cout
        << Colors::GREEN << "\n[" << Colors::DEF_COLOR << qxfile_src
        << Colors::GREEN << " and " << Colors::DEF_COLOR << qxfile_exe
        << Colors::GREEN <<" are successfully created]" << Colors::GREY
        << "\n[Press Enter to exit]" << Colors::DEF_COLOR << "\n";

    std::cin.get();
    ANSI_SEQ_OFF;
    return EXIT_SUCCESS;
}
