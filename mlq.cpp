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
        {"P1", 0, 8, 2, 8, 0, 0, 0},
        {"P2", 1, 4, 1, 4, 0, 0, 0},
        {"P3", 2, 9, 3, 9, 0, 0, 0},
        {"P4", 3, 5, 4, 5, 0, 0, 0}
    };

    // Initialize remaining time and last_executed_time
    for (auto &p : processes) {
        p.remaining_time = p.burst_time;
        p.last_executed_time = p.arrival_time;
    }

    std::queue<Process*> high_queue; // RR, quantum = 4
    std::queue<Process*> low_queue;  // FCFS
    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;
    int quantum = 4;

    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    while (!remaining.empty() || !high_queue.empty() || !low_queue.empty()) {
        // Add arrived processes to appropriate queues
        for (auto it = remaining.begin(); it != remaining.end();) {
            if ((*it)->arrival_time <= current_time) {
                if ((*it)->priority < 3) // High priority
                    high_queue.push(*it);
                else
                    low_queue.push(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        if (!high_queue.empty()) {
            // Round Robin for high-priority
            Process* p = high_queue.front();
            high_queue.pop();

            int time_run = std::min(quantum, p->remaining_time);

            // Update waiting time
            if (p->burst_time == p->remaining_time)
                p->waiting_time += current_time - p->arrival_time;
            else
                p->waiting_time += current_time - p->last_executed_time;

            gantt.push_back({p->id, time_run});
            current_time += time_run;
            p->remaining_time -= time_run;
            p->last_executed_time = current_time;

            // Add newly arrived processes during this quantum
            for (auto it = remaining.begin(); it != remaining.end();) {
                if ((*it)->arrival_time <= current_time) {
                    if ((*it)->priority < 3)
                        high_queue.push(*it);
                    else
                        low_queue.push(*it);
                    it = remaining.erase(it);
                } else {
                    ++it;
                }
            }

            // If not finished, push back to high queue
            if (p->remaining_time > 0) {
                high_queue.push(p);
            } else {
                p->turnaround_time = current_time - p->arrival_time;
            }
        } else if (!low_queue.empty()) {
            // FCFS for low-priority
            Process* p = low_queue.front();
            low_queue.pop();

            // Wait if CPU idle
            if (current_time < p->arrival_time)
                current_time = p->arrival_time;

            p->waiting_time = current_time - p->arrival_time;
            gantt.push_back({p->id, p->remaining_time});
            current_time += p->remaining_time;
            p->turnaround_time = p->waiting_time + p->remaining_time;
            p->remaining_time = 0;
        } else {
            // CPU idle
            current_time++;
        }
    }

    calculateMetrics(processes, current_time);
    printGantt(gantt);

    return 0;
}