#include "process_info.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

ProcessInfo GetProcessInfo(const std::string& pid) {
    ProcessInfo p;
    p.pid = std::stoi(pid);
    p.cpu = 0.0f;
    p.memory = 0.0f;
    p.name = "unknown";
    p.status = "unknown";

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