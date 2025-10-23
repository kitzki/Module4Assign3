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

int main() {
    std::vector<Process> processes = {
        {"P1", 0, 8, 2, 8, 0, 0},
        {"P2", 1, 4, 1, 4, 0, 0},
        {"P3", 2, 9, 3, 9, 0, 0},
        {"P4", 3, 5, 4, 5, 0, 0}
    };

    // Initialize remaining time
    for (auto &p : processes) p.remaining_time = p.burst_time;

    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;
    Process* current = nullptr;
    int time_slice = 0; // Track duration of current process in gantt

    while (true) {
        // Find all arrived processes with remaining time > 0
        std::vector<Process*> arrived;
        for (auto &p : processes) {
            if (p.arrival_time <= current_time && p.remaining_time > 0)
                arrived.push_back(&p);
        }

        if (arrived.empty()) {
            // No process ready
            if (current) {
                // Finish current slice
                gantt.push_back({current->id, time_slice});
                current = nullptr;
                time_slice = 0;
            }
            current_time++;
            bool done = true;
            for (auto &p : processes) if (p.remaining_time > 0) done = false;
            if (done) break;
            continue;
        }

        // Select process with shortest remaining time
        Process* shortest = *std::min_element(arrived.begin(), arrived.end(),
            [](Process* a, Process* b){ return a->remaining_time < b->remaining_time; });

        // If different from current, context switch
        if (current != shortest) {
            if (current) {
                // Save current slice
                gantt.push_back({current->id, time_slice});
            }
            current = shortest;
            time_slice = 0;
        }

        // Run for 1 unit time
        current->remaining_time--;
        time_slice++;
        current_time++;

        // If process finished, update metrics
        if (current->remaining_time == 0) {
            current->turnaround_time = current_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
            gantt.push_back({current->id, time_slice});
            current = nullptr;
            time_slice = 0;
        }
    }

    calculateMetrics(processes, current_time);
    printGantt(gantt);

    return 0;
}