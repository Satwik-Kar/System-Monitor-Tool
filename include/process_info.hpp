#pragma once

#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu;
    float memory;
    std::string status;
    unsigned long long active_jiffies;
};

std::vector<ProcessInfo> FetchRealProcessData();