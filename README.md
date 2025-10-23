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
````

> Add other `.cpp` files if implementing additional schedulers:
>
> ```bash
> g++ simulator.cpp fcfs.cpp rr.cpp -o simulator
> ```

---

## Usage

Basic command structure:

```bash
./simulator --scheduler <scheduler_type> [--input <file>] [--quantum <n>] [--random] [--num <n>] [--output <log_file>]
```

### Examples

1. **Round Robin (RR) with default quantum = 4**

```bash
./simulator --scheduler rr
```

2. **Round Robin with custom quantum**

```bash
./simulator --scheduler rr --quantum 3
```

3. **FCFS with input file**

```bash
./simulator --scheduler fcfs --input tasks.txt
```

4. **Generate 10 random processes**

```bash
./simulator --scheduler rr --random
```

5. **Generate 20 random processes**

```bash
./simulator --scheduler rr --random --num 20
```

6. **Log results to a file**

```bash
./simulator --scheduler rr --random --output log.txt
```

---

## Input File Format

* Optional `.txt` file with process information.
* Each line represents one process:

```
<id> <arrival_time> <burst_time> <priority>
```

**Example (`tasks.txt`):**

```
P1 0 8 2
P2 1 4 1
P3 2 9 3
P4 3 5 4
```

---

## Random Process Generation

* Use `--random` to generate random processes.
* By default, generates 10 processes.
* Use `--num <n>` to generate a custom number of processes.

---

## Output

* **Gantt Chart** – Shows the execution order of processes.

* **Metrics** – Displays:

  * Average waiting time
  * Average turnaround time
  * CPU utilization
  * Throughput

* Optional log file can store the Gantt chart and metrics.

---

## Future Enhancements

* Implement additional schedulers: **CFS, Lottery, Multilevel Queue**.
* Add **I/O simulation** for more realistic scenarios.
* Support **deadlines or weights** from input file.
* Simulate a **multi-core CPU environment**.
* Add **GUI visualization** of Gantt chart.

---

## Project Structure

```
simulator.cpp      # Main simulator and RR scheduler
fcfs.cpp           # FCFS scheduler (example)
srtf.cpp           # SRTF scheduler (optional)
priority.cpp       # Priority scheduler (optional)
edf.cpp            # EDF scheduler (optional)
tasks.txt          # Sample input file
README.md          # Project documentation
```
