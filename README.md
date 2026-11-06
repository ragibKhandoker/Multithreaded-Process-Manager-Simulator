<div align="center">

# 🧵 Multithreaded Process Manager Simulator

**An OS process manager simulator written in C with POSIX threads: fork, exit, wait, kill and a monitor thread working on one shared process table.**

<br/>

<a href="https://multithreaded-process-manager-simul.vercel.app" target="_blank">
  <img src="https://img.shields.io/badge/VIEW%20LIVE%20WEBSITE-7c5cff?style=for-the-badge&logo=vercel&logoColor=white" alt="View Live Website" height="45"/>
</a>

<br/><br/>

🌐 **Live URL:** [multithreaded-process-manager-simul.vercel.app](https://multithreaded-process-manager-simul.vercel.app)

<br/>

[![Vercel](https://img.shields.io/badge/Deployed%20on-Vercel-000000?style=flat-square&logo=vercel)](https://multithreaded-process-manager-simul.vercel.app)
![Status](https://img.shields.io/badge/Website-Online-22c55e?style=flat-square)
![C](https://img.shields.io/badge/Language-C-22d3ee?style=flat-square&logo=c)
![pthreads](https://img.shields.io/badge/Threads-POSIX%20pthreads-f59e0b?style=flat-square)
![Platform](https://img.shields.io/badge/Platform-Linux%20%2F%20WSL-64748b?style=flat-square&logo=linux)

<br/>

[**Purpose**](https://multithreaded-process-manager-simul.vercel.app/#purpose) ·
[**Architecture**](https://multithreaded-process-manager-simul.vercel.app/#architecture) ·
[**API**](https://multithreaded-process-manager-simul.vercel.app/#api) ·
[**Interactive Demo**](https://multithreaded-process-manager-simul.vercel.app/#demo) ·
[**Output**](https://multithreaded-process-manager-simul.vercel.app/#output) ·
[**Run it**](https://multithreaded-process-manager-simul.vercel.app/#run)

</div>

---

## 🌐 Live Website

The project has its own website, deployed on **Vercel**, that explains everything visually:

> ### 👉 [https://multithreaded-process-manager-simul.vercel.app](https://multithreaded-process-manager-simul.vercel.app)

| Section | What you will see |
| --- | --- |
| 🎯 [**Purpose**](https://multithreaded-process-manager-simul.vercel.app/#purpose) | Why the project exists and which OS concepts it teaches |
| 🧱 [**Architecture**](https://multithreaded-process-manager-simul.vercel.app/#architecture) | Diagram of the worker threads, process table, mutex and monitor thread, plus the PCB struct |
| 🧩 [**API**](https://multithreaded-process-manager-simul.vercel.app/#api) | Every simulated system call and script command, with return values |
| ▶️ [**Interactive Demo**](https://multithreaded-process-manager-simul.vercel.app/#demo) | Step-by-step replay of a sample run: watch the process table change over time |
| 📊 [**Output**](https://multithreaded-process-manager-simul.vercel.app/#output) | Console output and the actual `snapshots.txt`, with an explanation of the results |
| 🛠️ [**Run it**](https://multithreaded-process-manager-simul.vercel.app/#run) | Clone, compile and run instructions |

---

## 📌 Overview

This project is a **Multithreaded Process Manager Simulator** written in **C using POSIX Threads (pthreads)**. It simulates how an operating system manages processes using a shared process table and worker threads that run at the same time.

It does not create real OS processes. Each simulated process is a **Process Control Block (PCB)** in a shared table, and the simulator supports creating, terminating, waiting for, killing and monitoring these processes.

## 🎯 Purpose

* Understand the **process lifecycle**: `fork` → `exit` (zombie) → `wait` (reaped)
* Practise **multithreading** with pthreads
* Prevent race conditions with **mutexes** and coordinate threads with **condition variables**
* Simulate **OS-level process behaviour** in a safe environment that you can watch step by step

---

## ⚙️ Features

* 🧠 Simulated **process table** (max 64 processes, max 10 children per process)
* 🔄 Multiple **worker threads**, one per script file, running at the same time
* 🧩 Core process operations:
  * `pm_fork()`: create a child process
  * `pm_exit()`: terminate a process (it becomes a zombie)
  * `pm_wait()`: reap a zombie child
  * `pm_kill()`: force-terminate a process (exit status 9)
  * `pm_ps()`: print the process table
* 📊 **Monitor thread** that writes a snapshot to `snapshots.txt` after every change
* 🔐 A single `pthread_mutex_t` protects the table, and a `pthread_cond_t` wakes the monitor
* 📄 Each thread's behaviour is defined by a plain `.txt` script

---

## 🧱 Architecture

```
thread1.txt ─► Worker thread 1 ─┐
thread2.txt ─► Worker thread 2 ─┼─► pm_fork / pm_exit / pm_wait / pm_kill / pm_ps
threadN.txt ─► Worker thread N ─┘                │
                                                 ▼
                                   ┌─────────────────────────────┐
                                   │ Process Table  PCB table[64] │
                                   │ 🔒 pthread_mutex_t lock      │
                                   └─────────────────────────────┘
                                                 │ pthread_cond_signal
                                                 ▼
                                   Monitor thread ─► snapshots.txt
```

| Component | Role |
| --- | --- |
| **Process manager** | PCB table and helper functions (`find`, `add_child`, `remove_child`) |
| **Worker threads** | Each reads one script file and runs its commands |
| **Script interpreter** | Parses each command and calls the matching `pm_*` function |
| **Monitor thread** | Waits on a condition variable and logs a table snapshot whenever something changes |

---

## 📂 Project Structure

```
.
├── Project_Sec9_22301202_Group_9/
│   └── pm_sim/
│       ├── pm_sim.c        # Main implementation
│       ├── pm_sim          # Compiled Linux binary
│       ├── thread0.txt     # Empty sample script
│       ├── thread1.txt     # Sample script for worker thread 1
│       ├── thread2.txt     # Sample script for worker thread 2
│       └── snapshots.txt   # Output from the monitor thread
├── website/                # Project website (HTML + CSS + a little JS), deployed on Vercel
│   ├── index.html
│   ├── style.css
│   └── script.js
└── README.md
```

---

## 🛠️ Compilation & Usage

The program needs POSIX threads. Use Linux, macOS or **WSL** on Windows.

```bash
cd Project_Sec9_22301202_Group_9/pm_sim
gcc pm_sim.c -o pm_sim -pthread
./pm_sim thread1.txt thread2.txt
cat snapshots.txt
```

Each script file runs as its own worker thread.

> ℹ️ The monitor thread runs in an infinite loop by design, so you may need to press `Ctrl + C` after `=== All workers finished ===` appears.

---

## 📜 Script Command Format

| Command | Function | Description | Returns |
| --- | --- | --- | --- |
| `fork <ppid>` | `pm_fork` | Create a RUNNING child under a RUNNING parent | child PID, or -1 |
| `exit <pid> <status>` | `pm_exit` | Make the process a ZOMBIE and store its exit status | 0, or -1 |
| `wait <ppid> <pid\|-1>` | `pm_wait` | Reap a zombie child (`-1` = any child) and mark it DEAD | exit status, or -1 if no zombie child |
| `kill <pid>` | `pm_kill` | Force-terminate: ZOMBIE with exit status 9 | 0, or -1 |
| `sleep <ms>` | `usleep` | Pause only this worker thread | — |
| `ps` | `pm_ps` | Print the process table | — |

---

## 🔄 Process States

| State | Meaning |
| --- | --- |
| **RUNNING** | Active process, created by `fork` |
| **BLOCKED** | Defined for future use (`wait` is currently non-blocking) |
| **ZOMBIE** | Finished via `exit` or `kill`; its exit status is kept until the parent waits |
| **DEAD** | Reaped by the parent's `wait`; hidden from `ps` and snapshots |

```
RUNNING ──exit/kill──► ZOMBIE ──parent wait──► DEAD
```

---

## 📌 Example Run

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

**Timeline**

| Time | Thread | Action | Result |
| --- | --- | --- | --- |
| 0 ms | T1 | `fork 1` | PID 2 created |
| 100 ms | T2 | `fork 1` | PID 3 created |
| 200 ms | T1 | `fork 1` | PID 4 created |
| 200 ms | T1 | `wait 1 -1` | returns **-1** (no child has exited yet) |
| 400 ms | T2 | `exit 2 10` | PID 2 → **ZOMBIE (10)** |

**Final `snapshots.txt` entry** (actual output)

```
Process Table Snapshot:
PID      PPID     STATE        EXIT_STATUS
1        0        RUNNING      -
2        1        ZOMBIE       10
3        1        RUNNING      -
4        1        RUNNING      -
```

No process waits for PID 2 after it exits, so it **stays a zombie**. A real OS behaves the same way when a parent never calls `wait()`. Because threads run at the same time, console line order can differ slightly from run to run.

---

## ⚠️ Notes & Limits

* Maximum **64** processes and **10** children per process
* PIDs are assigned in increasing order and never reused
* DEAD processes are not shown in `ps` or in snapshots
* Every `pm_*` function holds the mutex while it changes the table

## 🚀 Future Improvements

* Blocking `wait` that uses the `BLOCKED` state
* Re-parenting orphans to init (PID 1)
* Clean shutdown of the monitor thread
* Priority-based scheduling and timestamps in the log

---

## 👨‍💻 Authors

**Group 9 · Section 9 · ID 22301202**, Operating Systems course project
GitHub: [@ragibKhandoker](https://github.com/ragibKhandoker)

## 📄 License

This project is for academic purposes.
