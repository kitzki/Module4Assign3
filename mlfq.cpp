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

    // Initialize
    for (auto &p : processes) {
        p.remaining_time = p.burst_time;
        p.last_executed_time = p.arrival_time;
    }

    std::vector<std::queue<Process*>> queues(3); // 3 levels
    std::vector<int> quanta = {2, 4, 8}; // Time slices for each level
    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;

    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    const int PROMOTION_WAIT = 10; // Example: promote if waiting too long

    while (!remaining.empty() || !queues[0].empty() || !queues[1].empty() || !queues[2].empty()) {
        // Add newly arrived processes to top queue
        for (auto it = remaining.begin(); it != remaining.end();) {
            if ((*it)->arrival_time <= current_time) {
                queues[0].push(*it);
                it = remaining.erase(it);
            } else {
                ++it;
            }
        }

        Process* current = nullptr;
        int level = -1;

        // Pick highest non-empty queue
        for (int i = 0; i < 3; i++) {
            if (!queues[i].empty()) {
                current = queues[i].front();
                queues[i].pop();
                level = i;
                break;
            }
        }

        if (current) {
            int time_run = std::min(quanta[level], current->remaining_time);

            // Update waiting time
            if (current->burst_time == current->remaining_time)
                current->waiting_time += current_time - current->arrival_time;
            else
                current->waiting_time += current_time - current->last_executed_time;

            gantt.push_back({current->id, time_run});
            current_time += time_run;
            current->remaining_time -= time_run;
            current->last_executed_time = current_time;

            // Add newly arrived during this time slice
            for (auto it = remaining.begin(); it != remaining.end();) {
                if ((*it)->arrival_time <= current_time) {
                    queues[0].push(*it);
                    it = remaining.erase(it);
                } else {
                    ++it;
                }
            }

            // Process finished
            if (current->remaining_time == 0) {
                current->turnaround_time = current_time - current->arrival_time;
            } else {
                // Demote if used full quantum and not finished
                if (time_run == quanta[level] && level < 2)
                    queues[level + 1].push(current);
                else
                    queues[level].push(current); // stay in same level
            }

            // Promote if waiting too long (optional)
            for (int i = 1; i < 3; i++) {
                std::queue<Process*> temp;
                while (!queues[i].empty()) {
                    Process* p = queues[i].front();
                    queues[i].pop();
                    if (current_time - p->last_executed_time >= PROMOTION_WAIT)
                        queues[i - 1].push(p);
                    else
                        temp.push(p);
                }
                queues[i] = temp;
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