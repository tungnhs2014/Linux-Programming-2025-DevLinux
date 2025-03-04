# GENERAL KNOWLEDGE

## 1. Introduction to Makefile
### 1.1. What is Makefile? 
- Makefile is a script used by the make build system to automate the compilation and management of projects. It contains:
  + Project structure information (files and their dependencies).
  + Commands for compiling, linking, and cleaning up files.
  + Rules that define how files should be generated.
- The make program reads the Makefile, determines which files need to be updated, and executes the necessary commands to build the project efficiently.
- **Naming Conventions:**
  + `Makefile` (recommended, as `make` searches for this by default).
  + `makefile` (less common but still works).
  + `*.mk` (used in modular or included Makefiles).
![Image](https://github.com/user-attachments/assets/a6448848-a377-4f52-8ab4-a7540e31de65)

### 1.2. How to run make?
- The `make` command executes **shell commands** in Linux to update one or more **target files** (typically a program or an executable).
- If the `-f` option is not provided, **make** automatically looks for a file named **Makefile** or **makefile** in the current directory.
- It checks **dependencies** and executes the required commands **only if changes** are detected, optimizing the build process.
- For more details, refer to the manual: `man make`
![Image](https://github.com/user-attachments/assets/68a634ea-35e1-4c89-a6cf-dcea405c3e70)
![Image](https://github.com/user-attachments/assets/d7f86721-6a11-495b-9564-ff9a3177135d)
![Image](https://github.com/user-attachments/assets/2e931dbf-b370-476a-a428-7f65927efdc4)

### 1.3. Variables in Makefile
**Why Use Variables?**
- Variables help **generalize** the `Makefile`, avoiding hardcoding specific file names.
- They make the `Makefile` **easier to modify and maintain**.
- To access the value of a variable, use the syntax: `$(VARIABLE_NAME)`
- We can print the value of a variable using an echo command inside a special target:
```  
  print:
      echo $(CFLAGS)
```
**Common Compiler & Linker Flags**
```
  CFLAGS += -I./include -DDEBUG -Wall -g
  LDFLAGS += -L./lib -lm
```
- `CFLAGS`: Flags passed to the C compiler (gcc).
  + `I./include` → Adds `include/` as a header file search path.
  + `-DDEBUG` → Defines a preprocessor macro DEBUG.
  + `-Wall` → Enables all important warnings.
  + `-g` → Generates debugging symbols for gdb.
    
- `LDFLAGS`: Flags passed to the **linker** (`ld`).
  + `-L./lib` → Adds `lib/` as a **library search path**.
  + `-lm` → Links the **math library** (`libm.so`).
![Image](https://github.com/user-attachments/assets/5db5072e-7e56-4bc8-ace0-1f6731e08b48)

**Automatic Variables**
- Automatic variables are special predefined variables in Makefile that help streamline rule definitions by referring to targets, dependencies, and commands dynamically.
![Image](https://github.com/user-attachments/assets/809cc837-1c27-4510-bd1a-79c4db3a2c1b)

### 1.4 Makefile Functions
- Makefile functions are built-in utilities that process strings, manipulate files, and execute shell commands. They help make Makefiles more dynamic and reusable by avoiding hardcoded values.
- **Most Common Makefile Functions**
![Image](https://github.com/user-attachments/assets/9fc44982-dfb8-4881-9769-23043b442c56)

---
## 2. The Process of Compiling C Program

---
## 3. Static Library and Shared Library in Linux
