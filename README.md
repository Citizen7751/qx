# Quick Execution

It's a simple tool that turns system commands into clickable shortcuts and thus makes execution easier. After you've specified your executable's name and its commands, it will drop a C source file and its compiled version, so it can be executed right away. The reason you get the source file next to the executable is to be able to modify it later and recompile it.

//screenshot
//editable parts

---

## Compile with GCC

**Windows**
```
g++ -o qx.exe qx.cpp -s -O3
```

**Linux**
```
g++ -o qx qx.cpp -s -O3
```

---

## License

This software is under the GPLv3 License.
