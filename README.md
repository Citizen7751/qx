# Quick Execution

It's a simple tool that turns system commands into clickable shortcuts and thus makes execution easier.

---

## Usage

When opened, the program will ask for the executable's name and its commands. After they are given, it will drop a C sourcefile and its compiled version, so it can be executed right away. The reason you get the sourcefile next to the executable is to be able to modify it later and recompile it.

![](screenshot.png)

Configurable parts in ```qx.cpp``` are ```def_qx_cc```, ```def_qx_ofile_flag``` and ```def_qx_make_flags```. If you want to change the C compiler it should use, or the flags, modify these. The default configuration is for GCC.

---

## Build

**Requirements:** [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/) (or any other C compiler)

### Building on Windows:

GCC: ```g++ -o qx.exe qx.cpp -s -O3```\
Clang:```clang++ -o qx.exe qx.cpp```

### Building on Linux:

GCC: ```g++ -o qx qx.cpp -s -O3```\
Clang:```clang++ -o qx qx.cpp```

### Note
If you don't have any standalone compilers installed but have Microsoft Visual Studio, you can copy-paste the sourcefile into a new C/C++ project and execute it that way. However, the program itself has to use a C compiler that can easily be treated independently from its IDE. That's why I recommend GCC and Clang.

---

## License

This software is under the GPLv3 License.
