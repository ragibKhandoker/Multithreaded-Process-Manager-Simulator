# 🧵 Multithreaded Process Manager Simulator

## 📌 Overview

This project is a **Multithreaded Process Manager Simulator** implemented in **C using POSIX Threads (pthreads)**. It simulates how an operating system internally manages processes using a shared process table and concurrent worker threads.

Instead of real OS processes, this system uses **Process Control Blocks (PCBs)** to represent processes and supports operations like process creation, termination, waiting, and monitoring.

---

## ⚙️ Features

* 🧠 Simulated **Process Table** (max 64 processes)
* 🔄 Multiple **Worker Threads** executing concurrently
* 🧩 Core Process Operations:

  * `pm_fork()` – Create a new process
  * `pm_exit()` – Terminate a process
  * `pm_wait()` – Wait for child process
  * `pm_kill()` – Force terminate process
  * `pm_ps()` – Display process table
* 📊 **Monitor Thread** that logs snapshots automatically
* 🔐 Proper synchronization using **mutexes / condition variables**
* 📄 Script-driven execution using `.txt` files

---

## 🧱 System Components

* **Process Manager Module**

  * Maintains process table and PCB structures

* **Worker Threads**

  * Each thread executes commands from a script file

* **Script Interpreter**

  * Parses commands and calls corresponding functions

* **Monitor Thread**

  * Prints process table snapshots when updates occur

---

## 📂 Project Structure

```
.
├── pm_sim.c              # Main implementation
├── thread1.txt           # Sample input script
├── thread2.txt           # Sample input script
├── snapshots.txt         # Output file (generated)
└── README.md
```

---

## 🛠️ Compilation

Make sure you have GCC installed.

```bash
gcc pm_sim.c -o pm_sim -pthread
```

---

## ▶️ Usage

Run the program with one or more script files:

```bash
./pm_sim thread1.txt thread2.txt
```

Each script file represents a separate worker thread.

---

## 📜 Script Command Format

Each line in a script file contains one command:

| Command                         | Description                       |
| ------------------------------- | --------------------------------- |
| `fork <pid>`                    | Create child process under parent |
| `exit <pid> <status>`           | Terminate process                 |
| `wait <parent_pid> <child_pid>` | Wait for child (-1 = any child)   |
| `kill <pid>`                    | Force terminate process           |
| `sleep <ms>`                    | Pause thread execution            |
| `ps`                            | Print process table               |

---

## 📊 Output

* Process table snapshots are automatically written to:

```
snapshots.txt
```

* Format:

```
PID   PPID   STATE     EXIT_STATUS
----------------------------------
1     0      RUNNING   -
2     1      ZOMBIE    10
```

---

## 🔄 Process States

* **RUNNING** – Active process
* **WAITING** – Waiting for child
* **ZOMBIE** – Terminated but not yet collected
* **TERMINATED** – Removed from system

---

## ⚠️ Notes

* Maximum number of processes: **64**
* PIDs are assigned incrementally
* Terminated processes are not shown in snapshots
* Proper synchronization is required to avoid race conditions

---

## 🎯 Learning Objectives

* Understand **process lifecycle management**
* Learn **multithreading with pthreads**
* Practice **synchronization (mutex, condition variables)**
* Simulate **OS-level process behavior**

---

## 📌 Example

**thread1.txt**

```
fork 1
sleep 200
fork 1
wait 1 -1
ps
```

**thread2.txt**

```
sleep 100
fork 1
sleep 300
exit 2 10
ps
```

---

## 🚀 Future Improvements

* Add priority-based scheduling
* Implement process queues
* Visual UI for process table
* Logging with timestamps

---

## 👨‍💻 Author

* Your Name Here

---

## 📄 License

This project is for academic purposes.
