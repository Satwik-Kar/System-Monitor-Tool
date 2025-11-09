#include "process_info.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

ProcessInfo GetProcessInfo(const std::string& pid) {
    ProcessInfo p;
    p.pid = std::stoi(pid);
    p.cpu = 0.0f;
    p.memory = 0.0f;
    p.name = "unknown";
    p.status = "unknown";
    p.active_jiffies = 0;

    std::string status_path = "/proc/" + pid + "/status";
    std::ifstream status_file(status_path);
    std::string line;

    if (status_file.is_open()) {
        while (std::getline(status_file, line)) {
            if (line.rfind("Name:", 0) == 0) {
                p.name = line.substr(line.find_first_not_of(" \t", 5));
            } else if (line.rfind("State:", 0) == 0) {
                p.status = line.substr(line.find_first_not_of(" \t", 6));
            } else if (line.rfind("VmRSS:", 0) == 0) {
                p.memory = std::stof(line.substr(line.find_first_not_of(" \t", 6))) / 1024.0f;
            }
        }
    }

    std::string stat_path = "/proc/" + pid + "/stat";
    std::ifstream stat_file(stat_path);
    std::string stat_line;
    if (std::getline(stat_file, stat_line)) {
        size_t close_paren = stat_line.find_last_of(')');
        if (close_paren != std::string::npos) {
            std::stringstream ss(stat_line.substr(close_paren + 2));
            std::string val;
            unsigned long long utime = 0, stime = 0;

            for (int i = 3; i <= 15; i++) {
                if (!(ss >> val)) break;
                if (i == 14) utime = std::stoull(val);
                if (i == 15) {
                    stime = std::stoull(val);
                    break;
                }
            }
            p.active_jiffies = utime + stime;
        }
    }

    return p;
}

std::vector<ProcessInfo> FetchRealProcessData() {
    std::vector<ProcessInfo> new_processes;
    std::string proc_dir = "/proc";

    for (const auto& entry : fs::directory_iterator(proc_dir)) {
        if (entry.is_directory()) {
            std::string filename = entry.path().filename().string();
            if (std::all_of(filename.begin(), filename.end(), ::isdigit)) {
                new_processes.push_back(GetProcessInfo(filename));
            }
        }
    }
    return new_processes;
}