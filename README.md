# Module4Assign3

This project is a CPU scheduling simulator written in C++. It allows you to simulate multiple CPU scheduling algorithms, visualize their Gantt charts, and calculate performance metrics such as average waiting time, turnaround time, CPU utilization, and throughput.

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

## Features
- Implements multiple CPU scheduling algorithms: RR, FCFS, SRTF, Priority, EDF, MLFQ, MLQ, Lottery, CFS, SJF.
- Gantt chart generation to visualize execution order.
- Metrics calculation:
  - Average waiting time
  - Average turnaround time
  - CPU utilization
  - Throughput
- Supports randomly generated processes.
- Supports input from a text file.
- Logging to an optional output file.

## Schedulers Implemented
- Round Robin (RR) – Preemptive, uses quantum.
- First-Come-First-Serve (FCFS) – Non-preemptive.
- Shortest Job First (SJF) – Non-preemptive.
- Shortest Remaining Time First (SRTF) – Preemptive.
- Priority Scheduling – Can implement aging to prevent starvation.
- Multilevel Queue (MLQ) – Predefined queues with different priorities.
- Multilevel Feedback Queue (MLFQ) – Dynamic queues based on behavior.
- Lottery Scheduling – Randomized selection based on tickets.
- Completely Fair Scheduler (CFS) – Uses virtual runtime.
- Earliest Deadline First (EDF) – Preemptive, uses deadlines.

Other schedulers like CFS or Lottery can be extended via the `Scheduler` base class.

## Getting Started

### Requirements
- C++17 compatible compiler (e.g., g++)
- Terminal or command line

### Compile
```bash
# Compile only the main simulator:
g++ simulator.cpp -o simulator

# Or compile with all scheduler files:
g++ simulator.cpp fcfs.cpp sjf.cpp srtf.cpp priority.cpp rr.cpp mlq.cpp mlfq.cpp lottery.cpp cfs.cpp edf.cpp -o simulator
````

## Usage

Basic command structure:

```bash
./simulator --scheduler <scheduler_type> [--input <file>] [--quantum <n>] [--random] [--num <n>] [--output <log_file>]
```

Examples:

```bash
./simulator --scheduler rr
./simulator --scheduler rr --quantum 3
./simulator --scheduler fcfs --input tasks.txt
./simulator --scheduler srtf --input tasks.txt
./simulator --scheduler rr --random
./simulator --scheduler rr --random --num 20
./simulator --scheduler rr --random --output log.txt
```

## Input File Format

Optional `.txt` file with process information. Each line represents one process:

```
<id> <arrival_time> <burst_time> <priority>
```

Example (`tasks.txt`):

```
P1 0 8 2
P2 1 4 1
P3 2 9 3
P4 3 5 4
```

## Random Process Generation

Use `--random` to generate random processes. By default generates 10 processes.
Use `--num <n>` to generate a custom number of processes.

## Output

* **Gantt Chart** – Shows execution order of processes.
* **Metrics** – Displays average waiting time, turnaround time, CPU utilization, and throughput.
* Optional log file can store the Gantt chart and metrics.

## Future Enhancements

* Implement additional schedulers: more variations of CFS, MLQ, MLFQ.
* Add I/O simulation for realistic scenarios.
* Support deadlines or weights from input file.
* Simulate multi-core CPU environment.
* Add GUI visualization of Gantt chart.

## Project Structure

```
simulator.cpp      # Main simulator and RR scheduler
fcfs.cpp           # FCFS scheduler
sjf.cpp            # SJF scheduler
srtf.cpp           # SRTF scheduler
priority.cpp       # Priority scheduler
rr.cpp             # Round Robin scheduler
mlq.cpp            # Multilevel Queue scheduler
mlfq.cpp           # Multilevel Feedback Queue scheduler
lottery.cpp        # Lottery scheduler
cfs.cpp            # Completely Fair Scheduler
edf.cpp            # EDF scheduler
tasks.txt          # Sample input file
README.md          # Project documentation
```

## Scheduler Cheat-Sheet

| Scheduler           | Command Example                                      | Notes                          |
| ------------------- | ---------------------------------------------------- | ------------------------------ |
| RR                  | ./simulator --scheduler rr                           | Default quantum = 4            |
| RR custom quantum   | ./simulator --scheduler rr --quantum 3               | Preemptive                     |
| FCFS                | ./simulator --scheduler fcfs --input tasks.txt       | Non-preemptive                 |
| SJF                 | ./simulator --scheduler sjf --input tasks.txt        | Non-preemptive                 |
| SRTF                | ./simulator --scheduler srtf --input tasks.txt       | Preemptive                     |
| Priority            | ./simulator --scheduler priority --input tasks.txt   | Can implement aging            |
| MLQ                 | ./simulator --scheduler mlq --input tasks.txt        | Multiple fixed-priority queues |
| MLFQ                | ./simulator --scheduler mlfq --input tasks.txt       | Dynamic feedback queues        |
| Lottery             | ./simulator --scheduler lottery --input tasks.txt    | Randomized selection           |
| CFS                 | ./simulator --scheduler cfs --input tasks.txt        | Completely Fair Scheduler      |
| EDF                 | ./simulator --scheduler edf --input tasks.txt        | Preemptive, uses deadlines     |
| Random processes    | ./simulator --scheduler rr --random                  | Generates 10 random processes  |
| Random custom count | ./simulator --scheduler rr --random --num 20         | Generates 20 random processes  |
| Log output          | ./simulator --scheduler rr --random --output log.txt | Logs metrics & Gantt chart     |

```
