#pragma once

#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu;
    float memory;
    std::string status;
};

std::vector<ProcessInfo> FetchRealProcessData();