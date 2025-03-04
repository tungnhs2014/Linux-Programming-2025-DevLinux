## 🏗️ Exercise 2: Creating a Shared Library (.so)

## 📚 Shared Library Build Guide

### **1️⃣ Project Structure**
```
├── 📂 bin/                   
│   └── exam                  
├── 📂 inc/                  
│   ├── calc_advance.h
│   └── calc_basic.h
├── 📂 lib/                 
│   └── 📂 shared_lib/        
│       └── libcalc.so       
├── 📂 obj/                   
│   ├── calc_advance.o
│   ├── calc_basic.o
│   └── main.o
├── 📂 src/                  
│   ├── calc_advance.c
│   ├── calc_basic.c
│   └── main.c
├── Makefile                            

```

### **2️⃣ Build Instructions**
#### **🔹 2.1 Compile & Create Shared Library**
    make all
- This will:
  + Compile `.c` files in src/ into **object files** `(.o)` in `obj/.` using the `-fPIC` flag to generate position-independent code.
  + Create a **shared library** `(libcalc.so)` inside `lib/shared_lib/.`
  + Generate the executable `(exam)` in `bin/.`, linking it with the shared library

#### **🔹 2.2 Clean Build Files**
    make clean
- This deletes `obj/`, `bin/`, and `lib/shared_lib/`.

---
### 3️⃣ Running the Executable
    export LD_LIBRARY_PATH=$PWD/lib/shared_lib:$LD_LIBRARY_PATH
    ./bin/exam

- Since exam depends on `libcalc.so`, you need to set the environment variable: 
    `export LD_LIBRARY_PATH=$PWD/lib/shared_lib:$LD_LIBRARY_PATH`

