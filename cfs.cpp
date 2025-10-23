#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <random>

struct Process {
    std::string id;
    int arrival_time;
    int burst_time;
    int priority;       // Lower number = higher priority
    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int last_executed_time;
    double vruntime;    // Virtual runtime for CFS
};

// Function to calculate average waiting/turnaround time and CPU utilization
void calculateMetrics(std::vector<Process>& processes, int total_time) {
    double avg_wait = 0, avg_turn = 0;
    for (auto& p : processes) {
        avg_wait += p.waiting_time;
        avg_turn += p.turnaround_time;
    }
    avg_wait /= processes.size();
    avg_turn /= processes.size();
    
    double cpu_util = (double)total_time / total_time * 100; // Simplified; adjust if idle
    std::cout << "Avg Waiting Time: " << avg_wait << "\n";
    std::cout << "Avg Turnaround Time: " << avg_turn << "\n";
    std::cout << "CPU Utilization: " << cpu_util << "%\n";
}

// Function to print text-based Gantt chart
void printGantt(const std::vector<std::pair<std::string, int>>& gantt) {
    std::cout << "Gantt Chart: ";
    for (auto& entry : gantt) {
        std::cout << entry.first << "(" << entry.second << ") ";
    }
    std::cout << "\n";
}

struct CFSCompare {
    bool operator() (const Process* a, const Process* b) {
        return a->vruntime > b->vruntime; // Min-heap
    }
};

int main() {
    std::vector<Process> processes = {
        {"P1", 0, 8, 2, 8, 0, 0, 0, 0.0},
        {"P2", 1, 4, 1, 4, 0, 0, 0, 0.0},
        {"P3", 2, 9, 3, 9, 0, 0, 0, 0.0},
        {"P4", 3, 5, 4, 5, 0, 0, 0, 0.0}
    };

    for (auto &p : processes) {
        p.remaining_time = p.burst_time;
        p.last_executed_time = p.arrival_time;
        p.vruntime = 0.0;
    }

    std::priority_queue<Process*, std::vector<Process*>, CFSCompare> ready_queue;
    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;
    const int SLICE = 1; // 1 time unit slice for simulation

    while (!remaining.empty() || !ready_queue.empty()) {
        // Add newly arrived processes
        for (auto it = remaining.begin(); it != remaining.end();) {
            if ((*it)->arrival_time <= current_time) {
                ready_queue.push(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        if (!ready_queue.empty()) {
            Process* p = ready_queue.top();
            ready_queue.pop();

            int time_run = std::min(SLICE, p->remaining_time);

            // Update waiting time
            if (p->burst_time == p->remaining_time)
                p->waiting_time += current_time - p->arrival_time;
            else
                p->waiting_time += current_time - p->last_executed_time;

            gantt.push_back({p->id, time_run});
            current_time += time_run;
            p->remaining_time -= time_run;
            p->last_executed_time = current_time;

            // Update vruntime: weight = priority, lower priority = heavier
            double weight = std::max(1, p->priority); // avoid divide by 0
            p->vruntime += (double)time_run / weight;

            if (p->remaining_time > 0) {
                ready_queue.push(p); // put back into heap
            } else {
                p->turnaround_time = current_time - p->arrival_time;
            }
        } else {
            // CPU idle
            current_time++;
        }
    }

    calculateMetrics(processes, current_time);
    printGantt(gantt);

    return 0;
}