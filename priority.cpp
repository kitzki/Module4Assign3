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

bool comparePriority(const Process* a, const Process* b) {
    return a->priority < b->priority; // Lower number = higher priority
}

int main() {
    std::vector<Process> processes = {
        {"P1", 0, 8, 2, 8, 0, 0},
        {"P2", 1, 4, 1, 4, 0, 0},
        {"P3", 2, 9, 3, 9, 0, 0},
        {"P4", 3, 5, 4, 5, 0, 0}
    };

    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;

    std::vector<Process*> ready_queue;
    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    const int AGING_INTERVAL = 1; // Increase priority for waiting processes each time unit

    while (!remaining.empty() || !ready_queue.empty()) {
        // Add arrived processes to ready_queue
        for (auto it = remaining.begin(); it != remaining.end();) {
            if ((*it)->arrival_time <= current_time) {
                ready_queue.push_back(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        // Apply aging to waiting processes
        for (auto &p : ready_queue) {
            if (p->arrival_time < current_time) {
                p->priority = std::max(0, p->priority - AGING_INTERVAL); // Lower number = higher priority
            }
        }

        if (!ready_queue.empty()) {
            // Pick process with highest priority (lowest number)
            auto highest_it = std::min_element(ready_queue.begin(), ready_queue.end(), comparePriority);
            Process* p = *highest_it;
            ready_queue.erase(highest_it);

            p->waiting_time = current_time - p->arrival_time;

            // Add to Gantt chart
            gantt.push_back({p->id, p->burst_time});

            // Advance time
            current_time += p->burst_time;

            // Turnaround time
            p->turnaround_time = p->waiting_time + p->burst_time;
        } else {
            // CPU idle
            current_time++;
        }
    }

    calculateMetrics(processes, current_time);
    printGantt(gantt);

    return 0;
}