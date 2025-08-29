# 🐚 Custom Linux Shell Implementation  

This project is a **custom shell program written in C**, designed to replicate core functionalities of a Unix/Linux shell environment. It demonstrates process management, signal handling, I/O redirection, and background job control — providing hands-on experience with low-level system programming.  

## 🚀 Features  
- **Command Execution**: Supports running user commands via `fork()` and `execvp()`.  
- **Foreground and Background Processes**:  
  - Run jobs in the foreground or append `&` to execute them in the background.  
  - Tracks and manages multiple background processes with process IDs.  
- **Signal Handling**:  
  - Handles **SIGINT (`Ctrl+C`)** to interrupt foreground processes.  
  - Handles **SIGTSTP (`Ctrl+Z`)** to toggle between normal mode and *foreground-only* mode.  
- **Built-in Commands**:  
  - `cd` – change directory (defaults to `$HOME` if no path provided).  
  - `status` – reports how the last foreground process exited (status code or signal).  
  - `exit` – terminates the shell, cleaning up all background processes.  
- **I/O Redirection**: Supports `<` (input redirection) and `>` (output redirection).  
- **Variable Expansion**: Expands `$$` into the shell’s process ID.  
- **Comment & Blank Line Handling**: Ignores lines beginning with `#` or empty input.  

## 🛠️ Technical Skills Demonstrated  
- **C Programming** with system-level libraries.  
- **Process Control** using `fork()`, `execvp()`, and `waitpid()`.  
- **Signal Management** with `sigaction()` for robust handling of `SIGINT` and `SIGTSTP`.  
- **File I/O** with `open()`, `dup2()`, and redirection logic.  
- **Dynamic Process Tracking** with arrays for background job management.  

## 📂 Use Case  
This shell serves as an educational exercise in building fundamental operating system components. It simulates the behavior of real shells like Bash, reinforcing concepts of multitasking, signals, and resource management.  

## 🎯 Motivation / Learning Outcomes

This project was developed to deepen my understanding of **operating systems** and **system-level programming**.  

Through building this shell, I learned:

- ⚙️ How processes are created, executed, and managed in **Linux/Unix**.  
- 🚦 The role of **signals** in process control and how to build robust signal handlers.  
- 🖥️ How **foreground vs. background execution** works in a shell.  
- 📂 Practical usage of **file descriptors** and how to implement **input/output redirection**.  
- 🔄 How to design a program that mimics real-world tools (like **Bash**) while reinforcing OS concepts.  

---

## **Compilation**

To compile the program, run:  

```bash
g++ gcc --std=gnu99 -g -o smallsh smallsh.c
```

This will generate an executable named `smallsh`.  

---

## **Running**

To start the shell, simply run:  

```bash
./smallsh
```

You will then see the shell prompt:  

```
:
```

At this prompt, you can enter commands just like in a regular shell.  

---

## **Features**

### **1. Built-in Commands**
- **`exit`**  
  - Terminates the shell.  
  - Waits for all background processes to finish before exiting.  

- **`cd`**  
  - Changes the current working directory.  
  - Usage: `cd [path]`  
  - If no path is given, changes to the user’s `$HOME` directory.  

- **`status`**  
  - Prints the exit status of the last foreground process.  
  - Reports either the **exit code** or the **signal** that terminated the process.  

---

### **2. Background Processes**
- Commands can be run in the background by appending **`&`** at the end.  
  Example:  
  ```bash
  sleep 10 &
  ```
  - The shell will immediately return control to the user.  
  - Background process IDs are tracked, and their completion is reported when they finish.  

---

### **3. Foreground-only Mode (SIGTSTP)**
- Pressing **`Ctrl+Z`** (`SIGTSTP`) toggles **foreground-only mode**:
  - When **enabled**, commands with `&` will run in the foreground instead of the background.  
  - The shell prints messages when entering and exiting this mode.  

---

### **4. Signal Handling**
- **`Ctrl+C` (SIGINT)**  
  - Foreground processes are terminated.  
  - Background processes ignore `SIGINT`.  
  - The shell itself ignores `SIGINT`.  

- **`Ctrl+Z` (SIGTSTP)**  
  - Toggles foreground-only mode (explained above).  

---

### **5. Input & Output Redirection**
- **Redirect input** using `< file`:  
  ```bash
  sort < input.txt
  ```
- **Redirect output** using `> file`:  
  ```bash
  ls > output.txt
  ```
- Files are opened with appropriate permissions:
  - Input (`<`) in **read-only** mode.  
  - Output (`>`) in **write/truncate/create** mode.  

---

### **6. Variable Expansion**
- The shell expands `$$` to the **process ID (PID)** of the shell.  
  Example:  
  ```bash
  echo "My shell PID is $$"
  ```
  might output:  
  ```
  My shell PID is 12345
  ```

---

## **Error Handling**
- Unknown commands result in:  
  ```
  Command not found
  ```
- Missing filenames for redirection print an error message.  
- Exceeding maximum background processes prints:  
  ```
  Error: Dude you have too many background processes
  ```

---

## **Example Usage**

```bash
: echo Hello World
Hello World

: sleep 5 &
background process id is 12345

: status
Exit value 0

: cat < input.txt > output.txt

: cd /usr
pwd is /usr

: exit
PID 12345 Exit status 0
```

---


## **Author**
This program was developed for practice with **Unix process management, signals, and system calls** in C.  
Exercise hosted by Oregon State University
