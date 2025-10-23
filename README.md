# Module4Assign3

This project is a **CPU scheduling simulator** written in C++. It allows you to simulate multiple CPU scheduling algorithms, visualize their Gantt charts, and calculate performance metrics such as **average waiting time**, **turnaround time**, **CPU utilization**, and **throughput**.

---

## Table of Contents

- [Features](#features)  
- [Schedulers Implemented](#schedulers-implemented)  
- [Getting Started](#getting-started)  
- [Usage](#usage)  
- [Input File Format](#input-file-format)  
- [Random Process Generation](#random-process-generation)  
- [Output](#output)  
- [Future Enhancements](#future-enhancements)  
- [Project Structure](#project-structure)  
- [Scheduler Cheat-Sheet](#scheduler-cheat-sheet)

---

## Features

- Implements multiple CPU scheduling algorithms: **RR, FCFS, SRTF, Priority, EDF** (others can be added).  
- Gantt chart generation to visualize execution order.  
- Metrics calculation:  
  - Average waiting time  
  - Average turnaround time  
  - CPU utilization  
  - Throughput  
- Supports **randomly generated processes**.  
- Supports **input from a text file**.  
- Logging to an optional output file.

---

## Schedulers Implemented

- **Round Robin (RR)** – Preemptive, uses quantum.  
- **First-Come-First-Serve (FCFS)** – Non-preemptive.  
- **Shortest Remaining Time First (SRTF)** – Preemptive.  
- **Priority Scheduling** – Can implement aging to prevent starvation.  
- **Earliest Deadline First (EDF)** – Preemptive, uses deadlines.  

> ⚠️ Other schedulers like **CFS, Lottery** can be added by extending the `Scheduler` base class.

---

## Getting Started

### Requirements

- C++17 compatible compiler (e.g., `g++`)  
- Terminal or command line  

### Compile

```bash
g++ simulator.cpp -o simulator
g++ simulator.cpp fcfs.cpp rr.cpp -o simulator

simulator.cpp      # Main simulator and RR scheduler
fcfs.cpp           # FCFS scheduler (example)
srtf.cpp           # SRTF scheduler (optional)
priority.cpp       # Priority scheduler (optional)
edf.cpp            # EDF scheduler (optional)
tasks.txt          # Sample input file
README.md          # Project documentation
