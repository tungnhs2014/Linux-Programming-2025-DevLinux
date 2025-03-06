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

### 1.4. Makefile Functions
- Makefile functions are built-in utilities that process strings, manipulate files, and execute shell commands. They help make Makefiles more dynamic and reusable by avoiding hardcoded values.
- **Most Common Makefile Functions**
![Image](https://github.com/user-attachments/assets/9fc44982-dfb8-4881-9769-23043b442c56)

---
## 2. The Process of Compiling a C Program

### 2.1. Pre-processing  
The pre-processing step performs the following tasks before actual compilation:  

- **Removes comments** from the source code.  
- **Expands macros** (`#define` and `#ifdef`).  
- **Expands include files** by replacing `#include` directives with the actual contents of the header files.  
- **Processes conditional compilation statements** (`#ifdef`, `#ifndef`, `#endif`, etc.).  
- **Generates an intermediate file** with a `.i` extension, containing the transformed source code without comments and with all macros and includes expanded.  

💡 **Command**  

```sh
gcc -E source.c -o source.i
```

### 2.2. Compilation
This stage translates the preprocessed C code into assembly language.  
- Translates the preprocessed `.i` file into assembly language.
- Performs `syntax analysis` and converts `C code `into `assembly` instructions.
- The output `.s` file contains human-readable assembly code.
  
💡 **Command**  
```sh
gcc -S source.i -o source.s
```

### 2.3. Assemply
In this stage, the assembly code is converted into machine code.  
- The file `*.s` in the previous stage continues to be used for this stage by using assembler.
- The output we get is a file `.o`. This is a file containing machine language level instructions.

💡 **Command**  
```sh
gcc -c source.s -o source.o
```

### 2.4. Linking
Combining Object Files to Create an Executable  

- The linking stage is where individual **object files (`.o`)** are combined to produce a complete executable program.  
- It connects object files with **static (`.a` / `.lib`)** or **shared (`.so` / `.dll`)** libraries.  
- The final output is a **executable file** (`a.out` by default, o `.exe` on Windows).

💡 **Command**  
```sh
gcc source.o -o output_executable
````
  
---

## 3. Static Library and Shared Library in Linux
### 3.1. What is a Library?
A **library** is a collection of precompiled code that can be reused in different programs. Instead of writing the same code multiple times, developers can use libraries to improve efficiency and maintainability. Libraries are mainly divided into two types:
- **Static Library** (`.a` on Linux, `.lib` on Windows)
- **Shared Library** (`.so` on Linux, `.dll` on Windows)
![Image](https://github.com/user-attachments/assets/75be86f5-3bfc-4b6f-84ff-ce256c4d415f)

### 3.2. Static Library
#### Definition
A **static library** is a set of compiled object files (`.o`) that are linked directly into an executable during the compilation phase.

#### How It Works
- All functions from the library are copied into the executable at **compile time**.
- The resulting executable is **self-contained**, meaning it does not require external dependencies at runtime.

#### Advantages
✅ Faster execution time (no runtime linking).  
✅ No dependency on external library files at runtime.  
✅ Ideal for standalone applications.

#### Disadvantages
❌ Increases the size of the executable because it contains copies of all required functions.  
❌ If the library is updated, the entire program needs to be **recompiled**.  

#### Example: Creating and Using a Static Library
```sh
# Step 1: Compile source files into object files
gcc -c libmath.c -o libmath.o

# Step 2: Create the static library
ar rcs libmath.a libmath.o

# Step 3: Link the static library with an executable
gcc main.c -o main_program -L. -lmath
```

### 3.3. Shared Library
#### Definition
A **shared library** is a collection of compiled functions that multiple programs can load at runtime.

#### How It Works
- The **linker** creates references to functions in the shared library, but does **not** copy them into the executable.
- At runtime, the **dynamic linker (`ld.so`)** loads the required library into memory.

#### Advantages
✅ Reduces the size of executables because the functions are not copied.  
✅ Multiple programs can share the same library in memory.  
✅ If the library is updated, all dependent programs automatically benefit without recompilation.

#### Disadvantages
❌ Slightly slower execution due to **dynamic linking** at runtime.  
❌ If the shared library is missing or incompatible, the program may fail to run.  

#### Example: Creating and Using a Shared Library
```sh
# Step 1: Compile source files as Position-Independent Code (PIC)
gcc -fPIC -c libmath.c -o libmath.o

# Step 2: Create the shared library
gcc -shared -o libmath.so libmath.o

# Step 3: Link the shared library with an executable
gcc main.c -o main_program -L. -lmath

# Step 4: Set the library path (if needed)
export LD_LIBRARY_PATH=.
```

### 3.4. Comparison Table

| **Properties**         | **Static Library** | **Shared Library** |
|------------------------|------------------|------------------|
| **Linking Time**       | Functions are copied into the executable at **compile time** (Static linking). | Functions are linked at **runtime** (Dynamic linking). |
| **Size**              | Larger executable (contains all library functions). | Smaller executable (functions are not copied). |
| **Memory Usage**      | Each executable has its own copy of the library. | Multiple programs share the same library in memory. |
| **Code Updates**      | Requires **recompilation** when the library is updated. | No need to recompile; updates apply automatically. |
| **Execution Speed**   | **Faster** because no dynamic linking is required. | **Slightly slower** due to runtime linking overhead. |
| **Dependencies**      | No external dependencies. | Requires the shared library to be present at runtime. |


### 3.5. When to Use Static vs. Shared Library?
| **Use Case** | **Preferred Library Type** |
|-------------|--------------------------|
| Standalone applications with no external dependencies | **Static Library** |
| System utilities or performance-critical software | **Static Library** |
| Large software projects with frequent updates | **Shared Library** |
| Applications that multiple programs will use | **Shared Library** |

### 3.6. Creating Libraries in Linux (Reference)
For more detailed steps and command-line instructions on creating static and shared libraries, refer to:  
👉 [Creating Libraries with GCC](https://renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/create-libraries/index)

