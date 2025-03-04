## 🏗️ Exercise 1: Creating a Static Library (.a)

## 📚 Static Library Build Guide

### **1️⃣ Project Structure**
├── bin
│   └── exam
├── inc
│   ├── calc_advance.h
│   └── calc_basic.h
├── lib
│   └── static_lib
│       └── libcalc.a
├── Makefile
├── obj
│   ├── calc_advance.o
│   ├── calc_basic.o
│   └── main.o
└── src
    ├── calc_advance.c
    ├── calc_basic.c
    └── main.c

---

### **2️⃣ Build Instructions**
#### **🔹 2.1 Compile & Create Static Library**
    make all
- This will:
  + Compile `.c` files in src/ into **object files** `(.o)` inside `obj/.`
  + Create a **static library** `(libcalc.a)` inside `lib/static_lib/.`
  + Build the executable `(exam)` in `bin/.`

#### **🔹 2.2 Clean Build Files**
    make clean
- This deletes `obj/`, `bin/`, and `lib/static_lib/`.


### 3️⃣ Running the Executable
    ./bin/exam

