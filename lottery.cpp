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

    std::vector<std::pair<std::string, int>> gantt;
    int current_time = 0;
    int quantum = 4; // Choose a time slice for lottery scheduling

    std::vector<Process*> remaining;
    for (auto &p : processes) remaining.push_back(&p);

    std::random_device rd;
    std::mt19937 gen(rd());

    while (!remaining.empty()) {
        // Build ticket list based on priority (lower number = higher priority)
        std::vector<Process*> tickets;
        for (auto &p : remaining) {
            if (p->arrival_time <= current_time && p->remaining_time > 0) {
                int num_tickets = std::max(1, 10 / p->priority); // Example: 10/priority
                for (int i = 0; i < num_tickets; i++)
                    tickets.push_back(p);
            }
        }

        if (tickets.empty()) {
            // CPU idle
            current_time++;
            continue;
        }

        // Randomly select a process
        std::uniform_int_distribution<> dis(0, tickets.size() - 1);
        Process* selected = tickets[dis(gen)];

        // Run for min(quantum, remaining_time)
        int time_run = std::min(quantum, selected->remaining_time);

        // Update waiting time
        if (selected->burst_time == selected->remaining_time)
            selected->waiting_time += current_time - selected->arrival_time;
        else
            selected->waiting_time += current_time - selected->last_executed_time;

        gantt.push_back({selected->id, time_run});
        current_time += time_run;
        selected->remaining_time -= time_run;
        selected->last_executed_time = current_time;

        if (selected->remaining_time == 0) {
            selected->turnaround_time = current_time - selected->arrival_time;
            // Remove from remaining
            auto it = std::find(remaining.begin(), remaining.end(), selected);
            if (it != remaining.end()) remaining.erase(it);
        }
    }

    calculateMetrics(processes, current_time);
    printGantt(gantt);

    return 0;
}