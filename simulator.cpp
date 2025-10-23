#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <map>
#include <memory>
#include <numeric>
#include <iomanip>

struct Process {
    std::string id;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int waiting_time = 0;
    int turnaround_time = 0;
    int deadline = 0;
    int vruntime = 0; // For CFS
    int weight = 1024; // For CFS (nice value 0)
    int last_run_time = 0;
    int io_burst = 0; // Optional I/O time
    bool in_io = false;
};

void calculateMetrics(const std::vector<Process>& processes, int total_time, 
                     double& avg_wait, double& avg_turn, double& cpu_util, double& throughput) {
    avg_wait = 0;
    avg_turn = 0;
    for (const auto& p : processes) {
        avg_wait += p.waiting_time;
        avg_turn += p.turnaround_time;
    }
    int n = processes.size();
    avg_wait /= n;
    avg_turn /= n;
    
    int total_burst = std::accumulate(processes.begin(), processes.end(), 0,
                                      [](int sum, const Process& p) { return sum + p.burst_time; });
    cpu_util = (total_time > 0) ? (double)total_burst / total_time * 100 : 0;
    throughput = (double)n / total_time;
}

void printGantt(const std::vector<std::pair<std::string, int>>& gantt) {
    std::cout << "Gantt Chart: ";
    for (const auto& entry : gantt) {
        std::cout << entry.first << "(" << entry.second << ") ";
    }
    std::cout << "\n";
}

void printResults(const std::vector<Process>& processes, int total_time,
                 const std::vector<std::pair<std::string, int>>& gantt) {
    double avg_wait, avg_turn, cpu_util, throughput;
    calculateMetrics(processes, total_time, avg_wait, avg_turn, cpu_util, throughput);
    printGantt(gantt);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Waiting Time: " << avg_wait << "\n";
    std::cout << "Average Turnaround Time: " << avg_turn << "\n";
    std::cout << "CPU Utilization: " << cpu_util << "%\n";
    std::cout << "Throughput: " << throughput << " processes/unit time\n";
}

// Abstract base class for schedulers
class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void schedule(std::vector<Process>& processes, 
                         std::vector<std::pair<std::string, int>>& gantt, 
                         int& total_time) = 0;
};

// 1. First-Come First-Served (FCFS)
class FCFSScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        int current_time = 0;
        for (auto& p : processes) {
            if (current_time < p.arrival_time) {
                current_time = p.arrival_time;
            }
            gantt.emplace_back(p.id, p.burst_time);
            current_time += p.burst_time;
            p.turnaround_time = current_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
        }
        total_time = current_time;
    }
};

// 2. Shortest Job First (SJF) - Non-preemptive
class SJFScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        int current_time = 0;
        std::vector<bool> completed(processes.size(), false);
        int completed_count = 0;
        
        while (completed_count < processes.size()) {
            int shortest = -1;
            int min_burst = INT32_MAX;
            
            for (size_t i = 0; i < processes.size(); ++i) {
                if (!completed[i] && processes[i].arrival_time <= current_time &&
                    processes[i].burst_time < min_burst) {
                    shortest = i;
                    min_burst = processes[i].burst_time;
                }
            }
            
            if (shortest == -1) {
                current_time++;
                continue;
            }
            
            auto& p = processes[shortest];
            gantt.emplace_back(p.id, p.burst_time);
            current_time += p.burst_time;
            p.turnaround_time = current_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            completed[shortest] = true;
            completed_count++;
        }
        total_time = current_time;
    }
};

// 3. Shortest Remaining Time First (SRTF) - Preemptive SJF
class SRTFScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) p.remaining_time = p.burst_time;
        
        int current_time = 0;
        int completed = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (completed < processes.size()) {
            int shortest = -1;
            int min_remaining = INT32_MAX;
            
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrival_time <= current_time && 
                    processes[i].remaining_time > 0 &&
                    processes[i].remaining_time < min_remaining) {
                    shortest = i;
                    min_remaining = processes[i].remaining_time;
                }
            }
            
            if (shortest == -1) {
                current_time++;
                continue;
            }
            
            auto& p = processes[shortest];
            if (last_id != p.id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = p.id;
                last_start = current_time;
            }
            
            p.remaining_time--;
            current_time++;
            
            if (p.remaining_time == 0) {
                p.turnaround_time = current_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                completed++;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 4. Round Robin (RR)
class RoundRobinScheduler : public Scheduler {
private:
    int quantum;
public:
    RoundRobinScheduler(int q) : quantum(q) {}
    
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) p.remaining_time = p.burst_time;
        std::queue<Process*> ready_queue;
        int current_time = 0;
        size_t idx = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (!ready_queue.empty() || idx < processes.size()) {
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                ready_queue.push(&processes[idx++]);
            }
            
            if (ready_queue.empty()) {
                if (idx < processes.size()) current_time = processes[idx].arrival_time;
                continue;
            }
            
            Process* current = ready_queue.front();
            ready_queue.pop();
            
            int run_time = std::min(quantum, current->remaining_time);
            if (last_id != current->id || last_start == -1) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = current->id;
                last_start = current_time;
            }
            
            current->remaining_time -= run_time;
            current_time += run_time;
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                ready_queue.push(&processes[idx++]);
            }
            
            if (current->remaining_time > 0) {
                ready_queue.push(current);
            } else {
                current->turnaround_time = current_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 5. Priority Scheduling (Non-preemptive) with Aging
class PriorityScheduler : public Scheduler {
private:
    int aging_interval;
public:
    PriorityScheduler(int aging = 5) : aging_interval(aging) {}
    
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        int current_time = 0;
        std::vector<bool> completed(processes.size(), false);
        int completed_count = 0;
        
        while (completed_count < processes.size()) {
            // Apply aging
            for (size_t i = 0; i < processes.size(); ++i) {
                if (!completed[i] && processes[i].arrival_time <= current_time) {
                    int wait = current_time - processes[i].arrival_time;
                    if (wait > 0 && wait % aging_interval == 0) {
                        processes[i].priority = std::max(1, processes[i].priority - 1);
                    }
                }
            }
            
            int highest = -1;
            int min_priority = INT32_MAX;
            
            for (size_t i = 0; i < processes.size(); ++i) {
                if (!completed[i] && processes[i].arrival_time <= current_time &&
                    processes[i].priority < min_priority) {
                    highest = i;
                    min_priority = processes[i].priority;
                }
            }
            
            if (highest == -1) {
                current_time++;
                continue;
            }
            
            auto& p = processes[highest];
            gantt.emplace_back(p.id, p.burst_time);
            current_time += p.burst_time;
            p.turnaround_time = current_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            completed[highest] = true;
            completed_count++;
        }
        total_time = current_time;
    }
};

// 6. Priority Scheduling (Preemptive)
class PreemptivePriorityScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) p.remaining_time = p.burst_time;
        
        int current_time = 0;
        int completed = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (completed < processes.size()) {
            int highest = -1;
            int min_priority = INT32_MAX;
            
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrival_time <= current_time && 
                    processes[i].remaining_time > 0 &&
                    processes[i].priority < min_priority) {
                    highest = i;
                    min_priority = processes[i].priority;
                }
            }
            
            if (highest == -1) {
                current_time++;
                continue;
            }
            
            auto& p = processes[highest];
            if (last_id != p.id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = p.id;
                last_start = current_time;
            }
            
            p.remaining_time--;
            current_time++;
            
            if (p.remaining_time == 0) {
                p.turnaround_time = current_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                completed++;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 7. Multilevel Queue Scheduler
class MultilevelQueueScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) p.remaining_time = p.burst_time;
        
        std::vector<std::queue<Process*>> queues(3);
        int current_time = 0;
        size_t idx = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (idx < processes.size() || 
               !queues[0].empty() || !queues[1].empty() || !queues[2].empty()) {
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                int q_level = (processes[idx].priority - 1) / 2;
                q_level = std::min(q_level, 2);
                queues[q_level].push(&processes[idx++]);
            }
            
            Process* current = nullptr;
            for (int i = 0; i < 3; ++i) {
                if (!queues[i].empty()) {
                    current = queues[i].front();
                    queues[i].pop();
                    break;
                }
            }
            
            if (!current) {
                if (idx < processes.size()) current_time = processes[idx].arrival_time;
                continue;
            }
            
            if (last_id != current->id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = current->id;
                last_start = current_time;
            }
            
            int run_time = std::min(2, current->remaining_time);
            current->remaining_time -= run_time;
            current_time += run_time;
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                int q_level = (processes[idx].priority - 1) / 2;
                q_level = std::min(q_level, 2);
                queues[q_level].push(&processes[idx++]);
            }
            
            if (current->remaining_time > 0) {
                int q_level = (current->priority - 1) / 2;
                q_level = std::min(q_level, 2);
                queues[q_level].push(current);
            } else {
                current->turnaround_time = current_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 8. Multilevel Feedback Queue Scheduler
class MFQScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) {
            p.remaining_time = p.burst_time;
            p.priority = 0;
        }
        
        std::vector<std::queue<Process*>> queues(3);
        int quantum[] = {2, 4, 8};
        int current_time = 0;
        size_t idx = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (idx < processes.size() || 
               !queues[0].empty() || !queues[1].empty() || !queues[2].empty()) {
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                queues[0].push(&processes[idx++]);
            }
            
            Process* current = nullptr;
            int q_level = -1;
            for (int i = 0; i < 3; ++i) {
                if (!queues[i].empty()) {
                    current = queues[i].front();
                    queues[i].pop();
                    q_level = i;
                    break;
                }
            }
            
            if (!current) {
                if (idx < processes.size()) current_time = processes[idx].arrival_time;
                continue;
            }
            
            if (last_id != current->id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = current->id;
                last_start = current_time;
            }
            
            int run_time = std::min(quantum[q_level], current->remaining_time);
            current->remaining_time -= run_time;
            current_time += run_time;
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                queues[0].push(&processes[idx++]);
            }
            
            if (current->remaining_time > 0) {
                int next_level = std::min(q_level + 1, 2);
                queues[next_level].push(current);
            } else {
                current->turnaround_time = current_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 9. Completely Fair Scheduler (CFS) - Simplified
class CFSScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) {
            p.remaining_time = p.burst_time;
            p.vruntime = 0;
        }
        
        auto cmp = [](Process* a, Process* b) { return a->vruntime > b->vruntime; };
        std::priority_queue<Process*, std::vector<Process*>, decltype(cmp)> ready_queue(cmp);
        
        int current_time = 0;
        size_t idx = 0;
        std::string last_id = "";
        int last_start = -1;
        
        while (!ready_queue.empty() || idx < processes.size()) {
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                ready_queue.push(&processes[idx++]);
            }
            
            if (ready_queue.empty()) {
                if (idx < processes.size()) current_time = processes[idx].arrival_time;
                continue;
            }
            
            Process* current = ready_queue.top();
            ready_queue.pop();
            
            if (last_id != current->id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = current->id;
                last_start = current_time;
            }
            
            int run_time = std::min(1, current->remaining_time);
            current->remaining_time -= run_time;
            current->vruntime += run_time * (1024 / current->weight);
            current_time += run_time;
            
            while (idx < processes.size() && processes[idx].arrival_time <= current_time) {
                ready_queue.push(&processes[idx++]);
            }
            
            if (current->remaining_time > 0) {
                ready_queue.push(current);
            } else {
                current->turnaround_time = current_time - current->arrival_time;
                current->waiting_time = current->turnaround_time - current->burst_time;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        total_time = current_time;
    }
};

// 10. Earliest Deadline First (EDF)
class EDFScheduler : public Scheduler {
public:
    void schedule(std::vector<Process>& processes, 
                 std::vector<std::pair<std::string, int>>& gantt, 
                 int& total_time) override {
        for (auto& p : processes) {
            p.remaining_time = p.burst_time;
            if (p.deadline == 0) {
                p.deadline = p.arrival_time + p.burst_time * 2;
            }
        }
        
        int current_time = 0;
        int completed = 0;
        std::string last_id = "";
        int last_start = -1;
        int missed_deadlines = 0;
        
        while (completed < processes.size()) {
            int earliest = -1;
            int min_deadline = INT32_MAX;
            
            for (size_t i = 0; i < processes.size(); ++i) {
                if (processes[i].arrival_time <= current_time && 
                    processes[i].remaining_time > 0 &&
                    processes[i].deadline < min_deadline) {
                    earliest = i;
                    min_deadline = processes[i].deadline;
                }
            }
            
            if (earliest == -1) {
                current_time++;
                continue;
            }
            
            auto& p = processes[earliest];
            if (last_id != p.id) {
                if (last_start != -1) {
                    gantt.emplace_back(last_id, current_time - last_start);
                }
                last_id = p.id;
                last_start = current_time;
            }
            
            p.remaining_time--;
            current_time++;
            
            if (p.remaining_time == 0) {
                p.turnaround_time = current_time - p.arrival_time;
                p.waiting_time = p.turnaround_time - p.burst_time;
                if (current_time > p.deadline) {
                    missed_deadlines++;
                }
                completed++;
            }
        }
        
        if (last_start != -1) {
            gantt.emplace_back(last_id, current_time - last_start);
        }
        
        if (missed_deadlines > 0) {
            std::cout << "Missed Deadlines: " << missed_deadlines << "\n";
        }
        total_time = current_time;
    }
};

std::vector<Process> loadProcesses(const std::string& filename) {
    std::vector<Process> processes;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << "\n";
        processes;
    }
    
    std::string id;
    int at, bt, pri;
    while (file >> id >> at >> bt >> pri) {
        processes.push_back({id, at, bt, pri, bt});
    }
    
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    
    return processes;
}

std::vector<Process> generateRandomProcesses(int num) {
    std::vector<Process> processes;
    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    
    for (int i = 0; i < num; ++i) {
        std::string id = "P" + std::to_string(i + 1);
        int at = gen() % 20;
        int bt = 1 + gen() % 10;
        int pri = 1 + gen() % 5;
        processes.push_back({id, at, bt, pri, bt});
    }
    
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
    
    return processes;
}

int main(int argc, char* argv[]) {
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; i += 2) {
        args[argv[i]] = (i + 1 < argc) ? argv[i + 1] : "";
    }
    
    std::string scheduler_type = args["--scheduler"];
    std::string input_file = args["--input"];
    std::string output_file = args["--output"];
    int quantum = std::stoi(args["--quantum"].empty() ? "4" : args["--quantum"]);
    int num_random = std::stoi(args["--num"].empty() ? "10" : args["--num"]);
    bool random = args.count("--random");
    
    std::vector<Process> processes;
    if (random) {
        processes = generateRandomProcesses(num_random);
    } else if (!input_file.empty()) {
        processes = loadProcesses(input_file);
    } else {
        processes = {
            {"P1", 0, 8, 2, 8},
            {"P2", 1, 4, 1, 4},
            {"P3", 2, 9, 3, 9},
            {"P4", 3, 5, 4, 5}
        };
    }
    
    if (processes.empty()) {
        std::cerr << "No processes loaded.\n";
        return 1;
    }
    
    std::unique_ptr<Scheduler> scheduler;
    if (scheduler_type == "fcfs") {
        scheduler = std::make_unique<FCFSScheduler>();
    } else if (scheduler_type == "sjf") {
        scheduler = std::make_unique<SJFScheduler>();
    } else if (scheduler_type == "srtf") {
        scheduler = std::make_unique<SRTFScheduler>();
    } else if (scheduler_type == "rr") {
        scheduler = std::make_unique<RoundRobinScheduler>(quantum);
    } else if (scheduler_type == "priority") {
        scheduler = std::make_unique<PriorityScheduler>();
    } else if (scheduler_type == "priority-preemptive") {
        scheduler = std::make_unique<PreemptivePriorityScheduler>();
    } else if (scheduler_type == "mlq") {
        scheduler = std::make_unique<MultilevelQueueScheduler>();
    } else if (scheduler_type == "mfq") {
        scheduler = std::make_unique<MFQScheduler>();
    } else if (scheduler_type == "cfs") {
        scheduler = std::make_unique<CFSScheduler>();
    } else if (scheduler_type == "edf") {
        scheduler = std::make_unique<EDFScheduler>();
    } else {
        std::cerr << "Unknown scheduler: " << scheduler_type << "\n";
        std::cerr << "Available: fcfs, sjf, srtf, rr, priority, priority-preemptive, mlq, mfq, cfs, edf\n";
        return 1;
    }
    
    std::vector<std::pair<std::string, int>> gantt;
    int total_time = 0;
    
    std::cout << "Running " << scheduler_type << " scheduler...\n";
    scheduler->schedule(processes, gantt, total_time);
    printResults(processes, total_time, gantt);
    
    if (!output_file.empty()) {
        std::ofstream log(output_file);
        if (log) {
            log << "Scheduler: " << scheduler_type << "\n";
            log << "Quantum: " << quantum << "\n\n";
            
            double avg_wait, avg_turn, cpu_util, throughput;
            calculateMetrics(processes, total_time, avg_wait, avg_turn, cpu_util, throughput);
            
            log << "Gantt Chart: ";
            for (const auto& entry : gantt) {
                log << entry.first << "(" << entry.second << ") ";
            }
            log << "\n\n";
            
            log << std::fixed << std::setprecision(2);
            log << "Average Waiting Time: " << avg_wait << "\n";
            log << "Average Turnaround Time: " << avg_turn << "\n";
            log << "CPU Utilization: " << cpu_util << "%\n";
            log << "Throughput: " << throughput << " processes/unit time\n";
            
            std::cout << "Results saved to " << output_file << "\n";
        }
    }
    
    return 0;

}