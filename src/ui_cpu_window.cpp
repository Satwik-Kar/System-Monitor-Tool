#include "ui_cpu_window.hpp"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

float getBaseCpuGhz()
{
    std::ifstream freq_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");

    if (!freq_file.is_open())
    {
        std::cerr << "Error: Could not open cpuinfo_max_freq" << std::endl;
        return 0.0f;
    }

    long long freq_khz = 0;
    freq_file >> freq_khz;
    freq_file.close();

    if (freq_khz == 0)
    {
        return 0.0f;
    }

    return (float)freq_khz / 1000000.0f;
}
float getCurrentCpuGhz()
{

    std::ifstream freq_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");

    if (!freq_file.is_open())
    {
        freq_file.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
        if (!freq_file.is_open())
        {
            std::cerr << "Error: Could not open CPU frequency file" << std::endl;
            return 0.0f;
        }
    }

    long long freq_khz = 0;
    freq_file >> freq_khz;
    freq_file.close();

    if (freq_khz == 0)
    {
        return 0.0f;
    }

    return (float)freq_khz / 1000000.0f;
}

struct CpuStats
{
    long long user = 0, nice = 0, system = 0, idle = 0,
              iowait = 0, irq = 0, softirq = 0, steal = 0;

    long long TotalIdle() const { return idle + iowait; }
    long long TotalActive() const { return user + nice + system + irq + softirq + steal; }
    long long Total() const { return TotalIdle() + TotalActive(); }
};

CpuStats getCpuStats()
{
    std::ifstream stat_file("/proc/stat");
    std::string line;
    std::getline(stat_file, line);
    stat_file.close();

    std::stringstream ss(line);
    std::string cpu_label;
    CpuStats stats;

    if (ss >> cpu_label >> stats.user >> stats.nice >> stats.system >> stats.idle >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal)
    {
        return stats;
    }

    return {};
}
struct CpuPercentages
{
    float totalUsage = 0.0f;
    float user = 0.0f;
    float system = 0.0f;
    float idle = 0.0f;
};

CpuPercentages getCpuUsagePercent()
{
    static CpuStats prevStats = getCpuStats();
    CpuStats currentStats = getCpuStats();
    long long totalDelta = currentStats.Total() - prevStats.Total();
    long long activeDelta = currentStats.TotalActive() - prevStats.TotalActive();

    long long userDelta = (currentStats.user + currentStats.nice) - (prevStats.user + prevStats.nice);
    long long systemDelta = (currentStats.system + currentStats.irq + currentStats.softirq) - (prevStats.system + prevStats.irq + prevStats.softirq);
    long long idleDelta = currentStats.TotalIdle() - prevStats.TotalIdle();
    float totalUsage = 0.0f;
    float userPercentage = 0.0f;
    float systemPercentage = 0.0f;
    float idlePercentage = 0.0f;
    CpuPercentages percentages;
    if (totalDelta > 0)
    {
        percentages.totalUsage = (float)activeDelta / (float)totalDelta * 100.0f;
    }
    if (userDelta > 0)
    {
        percentages.user = static_cast<float>(userDelta) / totalDelta * 100.0f;
    }
    if (systemDelta > 0)
    {
        percentages.system = static_cast<float>(systemDelta) / totalDelta * 100.0f;
    }
    if (idleDelta > 0)
    {
        percentages.idle = static_cast<float>(idleDelta) / totalDelta * 100.0f;
    }

    prevStats = currentStats;
    return percentages;
}

unsigned int getWorkingCoreCount()
{
    return std::thread::hardware_concurrency();
}

unsigned int getTotalPossibleCores()
{
    std::ifstream possible_file("/sys/devices/system/cpu/possible");
    if (!possible_file.is_open())
    {
        std::cerr << "Error: Could not open /sys/devices/system/cpu/possible" << std::endl;
        return 0;
    }

    std::string line;
    std::getline(possible_file, line);
    possible_file.close();

    size_t last_dash = line.rfind('-');
    if (last_dash == std::string::npos)
    {
        try
        {
            int core_index = std::stoi(line);
            return static_cast<unsigned int>(core_index + 1);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing core count from single value: " << e.what() << std::endl;
            return 0;
        }
    }

    try
    {
        int last_core_index = std::stoi(line.substr(last_dash + 1));
        return static_cast<unsigned int>(last_core_index + 1);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing core count from range: " << e.what() << std::endl;
        return 0;
    }
}

void RenderCPUWindow(sf::Texture &cpuTexture,
                     sf::Texture &speedIcon,
                     sf::Texture &clockIcon,
                     sf::Texture &userIcon,
                     sf::Texture &systemIcon,
                     sf::Texture &idleIcon,
                     ImFont *font)
{
    ImGui::PushFont(font);

    static float displayedCpuGhz = 0.0f;
    static float displayedCpuUsage = 0.0f;

    static float displayedCpuUserUsage = 0.0f;
    static float displayedCpuSystemUsage = 0.0f;
    static float displayedCpuIdleUsage = 0.0f;

    static auto lastUpdateTime = std::chrono::steady_clock::now();

    float totalCpuGhz = getBaseCpuGhz();

    const auto updateInterval = std::chrono::milliseconds(1000);
    auto now = std::chrono::steady_clock::now();

    CpuPercentages cpuPercentages = getCpuUsagePercent();
    static unsigned int workingCores = 0;
    static unsigned int totalCores = 0;
    if (now - lastUpdateTime > updateInterval)
    {

        displayedCpuGhz = getCurrentCpuGhz();
        displayedCpuUsage = cpuPercentages.totalUsage;
        displayedCpuUserUsage = cpuPercentages.user;
        displayedCpuSystemUsage = cpuPercentages.system;
        displayedCpuIdleUsage = cpuPercentages.idle;
        workingCores = getWorkingCoreCount();
        totalCores = getTotalPossibleCores();
        lastUpdateTime = now;
    }

    ImVec2 display = ImGui::GetIO().DisplaySize;
    if (display.x < 400.0f || display.y < 300.0f)
    {
        ImGui::PopFont();
        return;
    }

    float paddingCpu = std::clamp(display.x * 0.02f, 10.0f, 40.0f);
    float winW = std::max(220.0f, display.x * 0.48f - paddingCpu);
    float winH = std::max(250.0f, display.y * 0.35f);

    ImGui::SetNextWindowPos(ImVec2(paddingCpu, 80), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::Begin("CPU Usage", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Image((void *)(intptr_t)cpuTexture.getNativeHandle(), ImVec2(std::clamp(winH * 0.08f, 20.0f, 40.0f), std::clamp(winH * 0.08f, 20.0f, 40.0f)));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "CPU Usage");

    float dotSize = std::max(6.0f, winH * 0.05f);
    float spacing = std::clamp(winW * 0.12f, 40.0f, 120.0f);
    ImVec2 winPos = ImGui::GetWindowPos();
    float baseY = winPos.y + 18.0f;
    float startX = ImGui::GetWindowContentRegionMax().x - std::clamp(winW * 0.25f, 80.0f, 220.0f);

    ImDrawList *drawListDots = ImGui::GetWindowDrawList();
    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX, baseY), dotSize / 2, IM_COL32(0, 200, 200, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0, 1, 1, 1), "User");

    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX + spacing, baseY), dotSize / 2, IM_COL32(120, 180, 255, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + spacing + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0.7, 0.9, 1, 1), "System");

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 8));

    ImDrawList *chipDrawList = ImGui::GetWindowDrawList();
    ImVec2 chipPos = ImGui::GetCursorScreenPos();
    float chipHeight = std::max(36.0f, winH * 0.09f);
    float chipPaddingX = std::clamp(winW * 0.05f, 24.0f, 60.0f);
    float chip1Width = std::clamp(winW * 0.32f, 120.0f, 260.0f);
    float chip2Width = std::clamp(winW * 0.28f, 100.0f, 220.0f);
    char cpuGhz[16];
    snprintf(cpuGhz, sizeof(cpuGhz), "%.1f / %.1f GHz", displayedCpuGhz, totalCpuGhz);

    char cpuUsagePercentage[16];
    snprintf(cpuUsagePercentage, sizeof(cpuUsagePercentage), "%.1f %%", displayedCpuUsage);

    DrawChip(chipDrawList, chipPos, 10, 10, chip1Width, chipHeight, IM_COL32(23, 43, 58, 255), speedIcon, cpuUsagePercentage, ImVec4(0.3, 0.6, 0.8, 1));
    DrawChip(chipDrawList, chipPos, 20 + chip1Width, 10, chip2Width, chipHeight, IM_COL32(28, 55, 70, 255), clockIcon, cpuGhz, ImVec4(0.4, 0.8, 1, 1));

    ImGui::Dummy(ImVec2(0, std::clamp(winH * 0.02f, 8.0f, 24.0f)));

    static float userCPU[100] = {};
    static float systemCPU[100] = {};
    static int dataIndex = 0;
    static float elapsed = 0.0f;
    const float UPDATE_INTERVAL = 0.2f;

    elapsed += ImGui::GetIO().DeltaTime;
    if (elapsed >= UPDATE_INTERVAL)
    {
        elapsed = 0.0f;
        userCPU[dataIndex] = cpuPercentages.user;
        systemCPU[dataIndex] = cpuPercentages.system;
        dataIndex = (dataIndex + 1) % IM_ARRAYSIZE(userCPU);
    }

    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, std::max((float)100.0f, ImGui::GetContentRegionAvail().y * 0.28f));
    ImVec2 graphPos = ImGui::GetCursorScreenPos();
    ImDrawList *graph = ImGui::GetWindowDrawList();
    float cornerRadius = std::max(8.0f, graphSize.y * 0.12f);
    ImU32 bgColor = IM_COL32(15, 25, 35, 255);
    ImU32 borderColor = IM_COL32(35, 55, 75, 255);
    graph->AddRectFilled(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), bgColor, cornerRadius);
    graph->AddRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), borderColor, cornerRadius, 0, 2.0f);

    graph->PushClipRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), true);

    // Add vertical padding to the graph lines
    float y_padding = graphSize.y * 0.1f; // 10% padding top and bottom
    float y_range = graphSize.y * 0.8f;   // Lines will be drawn in the middle 80%
    float y_base = graphPos.y + y_padding;

    for (int i = 1; i < IM_ARRAYSIZE(userCPU); i++)
    {
        int i0 = (dataIndex + i - 1) % IM_ARRAYSIZE(userCPU);
        int i1 = (dataIndex + i) % IM_ARRAYSIZE(userCPU);
        float x0 = graphPos.x + ((i - 1) / 99.0f) * graphSize.x;
        float x1 = graphPos.x + (i / 99.0f) * graphSize.x;

        // Apply padding and scaling to y-values
        float y0u = y_base + y_range * (1.0f - std::clamp(userCPU[i0], 0.0f, 100.0f) / 100.0f);
        float y1u = y_base + y_range * (1.0f - std::clamp(userCPU[i1], 0.0f, 100.0f) / 100.0f);
        float y0s = y_base + y_range * (1.0f - std::clamp(systemCPU[i0], 0.0f, 100.0f) / 100.0f);
        float y1s = y_base + y_range * (1.0f - std::clamp(systemCPU[i1], 0.0f, 100.0f) / 100.0f);

        graph->AddLine(ImVec2(x0, y0u), ImVec2(x1, y1u), IM_COL32(0, 200, 200, 255), std::max(2.0f, graphSize.y * 0.022f));
        graph->AddLine(ImVec2(x0, y0s), ImVec2(x1, y1s), IM_COL32(120, 180, 255, 255), std::max(2.0f, graphSize.y * 0.022f));
    }
    graph->PopClipRect();

    ImGui::Dummy(graphSize);
    ImGui::Dummy(ImVec2(0, std::clamp(winH * 0.04f, 12.0f, 28.0f)));

    ImDrawList *chips = ImGui::GetWindowDrawList();
    ImVec2 chipBase = ImGui::GetCursorScreenPos();
    struct ChipInfo
    {
        sf::Texture *icon;
        float label;
        ImU32 color;
        ImVec4 textColor;
    };

    std::vector<ChipInfo> list = {
        {&userIcon, displayedCpuUserUsage, IM_COL32(23, 43, 58, 255), ImVec4(0.6, 0.9, 1, 1)},
        {&systemIcon, displayedCpuSystemUsage, IM_COL32(28, 55, 70, 255), ImVec4(0.4, 0.8, 1, 1)},
        {&idleIcon, displayedCpuIdleUsage, IM_COL32(40, 70, 90, 255), ImVec4(0.5, 0.8, 1, 1)},
        {&cpuTexture, (float)workingCores, IM_COL32(50, 90, 110, 255), ImVec4(0.7, 1, 0.9, 1)}};

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float xOffset = std::max(8.0f, contentWidth * 0.02f);
    float chipSpacing = std::max(8.0f, contentWidth * 0.015f);
    float height = std::max(40.0f, contentWidth * 0.06f);
    float paddingX = std::clamp(contentWidth * 0.05f, 24.0f, 80.0f);

    const char *chipPrefixes[] = {
        "User: ",
        "System: ",
        "Idle: ",
        "Available Cores: "};

    for (int i = 0; i < list.size(); ++i)
    {
        const auto &c = list[i];
        char formattedLabel[32];

        if (i < 3)
        {
            snprintf(formattedLabel, sizeof(formattedLabel), "%s%.1f%%", chipPrefixes[i], c.label);
        }
        else
        {
            snprintf(formattedLabel, sizeof(formattedLabel), "%s%d", chipPrefixes[i], static_cast<int>(c.label));
        }

        float textWidth = ImGui::CalcTextSize(formattedLabel).x;
        float width = std::min(contentWidth * 0.9f, textWidth + paddingX + std::max(36.0f, contentWidth * 0.06f));

        DrawChip(chips, chipBase, xOffset, 0, width, height, c.color, *c.icon, formattedLabel, c.textColor);

        xOffset += width + chipSpacing;
        if (xOffset > contentWidth)
            break;
    }

    ImGui::End();
    ImGui::PopFont();
}