# **Small Shell (smallsh)**

This project implements a **custom shell in C** called `smallsh`.  
It supports **foreground and background processes, signal handling, input/output redirection, and process control**, similar to a simplified Bash shell.  

---

## **Compilation**

To compile the program, run:  

```bash
gcc --std=gnu99 -g -o smallsh smallsh.c
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

## **Key Concepts Demonstrated**
- **Process creation** using `fork()` and `execvp()`.  
- **Signal handling** using `sigaction()`.  
- **Foreground vs background execution** with `waitpid()`.  
- **Input/output redirection** with `dup2()`.  
- **Dynamic process management** with arrays of background PIDs.  

---

## **Author**
This program was developed for practice with **Unix process management, signals, and system calls** in C.  
Exercise hosted by Oregon State University
