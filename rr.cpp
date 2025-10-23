#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>

struct Process {
    std::string id;
    int arrival_time;
    int burst_time;
    int priority;         // Lower number = higher priority
    int remaining_time;   // For preemptive
    int waiting_time;
    int turnaround_time;
    int last_executed_time;
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

int main() {
    std::vector<Process> processes = {
        {"P1", 0, 8, 2, 8, 0, 0},
        {"P2", 1, 4, 1, 4, 0, 0},
        {"P3", 2, 9, 3, 9, 0, 0},
        {"P4", 3, 5, 4, 5, 0, 0}
    };

    for (auto &p : processes) p.remaining_time = p.burst_time;

    std::queue<Process*> ready_queue;
    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;
    int quantum = 4;

    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    while (!remaining.empty() || !ready_queue.empty()) {
        // Add arrived processes to ready_queue
        for (auto it = remaining.begin(); it != remaining.end();) {
            if ((*it)->arrival_time <= current_time) {
                ready_queue.push(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        if (!ready_queue.empty()) {
            Process* p = ready_queue.front();
            ready_queue.pop();

            // Run for min(quantum, remaining_time)
            int time_run = std::min(quantum, p->remaining_time);

            if (p->burst_time == p->remaining_time) {
                p->waiting_time += current_time - p->arrival_time;
            } else {
                p->waiting_time += current_time - p->last_executed_time;
            }

            gantt.push_back({p->id, time_run});
            current_time += time_run;
            p->remaining_time -= time_run;
            p->last_executed_time = current_time;

            // Add newly arrived processes during this quantum
            for (auto it = remaining.begin(); it != remaining.end();) {
                if ((*it)->arrival_time <= current_time) {
                    ready_queue.push(*it);
                    it = remaining.erase(it);
                } else {
                    ++it;
                }
            }

            if (p->remaining_time > 0) {
                ready_queue.push(p);
            } else {
                // Turnaround time
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