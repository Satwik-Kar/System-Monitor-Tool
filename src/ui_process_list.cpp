#include "ui_process_list.hpp"
#include "process_info.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <thread>
#include <fstream>
#include <sstream>
#include <signal.h> // Added for kill()
#include <unistd.h> // Added for getpid()

static char searchBuffer[128] = "";
static int sortIndex = 0;
static int statusIndex = 0;

static const char* sortOptions[] = {"CPU %", "Memory %", "PID", "Name"};
static const char* statusOptions[] = {"All", "Running", "Sleeping", "Stopped"};


struct CpuStats
{
    long long user = 0, nice = 0, system = 0, idle = 0,
              iowait = 0, irq = 0, softirq = 0, steal = 0;

    long long TotalIdle() const { return idle + iowait; }
    long long TotalActive() const { return user + nice + system + irq + softirq + steal; }
    long long Total() const { return TotalIdle() + TotalActive(); }
};

static CpuStats getCpuStats()
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


void RenderProcessHeaderBar(sf::Texture& listIcon, sf::Texture& searchIcon,
                            sf::Texture& sortIcon, sf::Texture& statusIcon,
                            ImFont* font) {
    ImGui::PushFont(font);

    ImVec2 display = ImGui::GetIO().DisplaySize;
    float headerHeight = std::clamp(display.y * 0.06f, 40.0f, 80.0f);

    ImGui::SetNextWindowPos(ImVec2(0, display.y / 2));
    ImGui::SetNextWindowSize(ImVec2(display.x, headerHeight));

    ImGui::Begin("Processes Header", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings);

    float padding = display.x * 0.015f;
    float logoSize = headerHeight * 0.5f;
    float fieldHeight = headerHeight * 0.75f;
    float spacing = display.x * 0.008f;

    ImVec4 textColor = ImVec4(0.85f, 0.9f, 0.95f, 1.0f);
    ImVec4 fieldColor = ImVec4(0.09f, 0.13f, 0.16f, 1.0f);
    ImVec4 borderColor = ImVec4(0.18f, 0.23f, 0.28f, 1.0f);

    ImGui::SetCursorPos(ImVec2(padding, (headerHeight - logoSize) * 0.5f));
    ImGui::Image((void*)(intptr_t)listIcon.getNativeHandle(), ImVec2(logoSize, logoSize));
    ImGui::SameLine();
    ImGui::SetCursorPosY((headerHeight - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::TextColored(textColor, "Running Processes");

    float searchWidth = std::clamp(display.x * 0.22f, 180.0f, 260.0f);
    float comboWidth = std::clamp(display.x * 0.13f, 110.0f, 150.0f);

    float totalWidth = searchWidth + 2 * comboWidth + 2 * spacing;
    float startX = display.x - totalWidth - padding;
    
    float centerY = (headerHeight - fieldHeight) * 0.5f;

    ImGui::SetCursorPos(ImVec2(startX, centerY));

    ImGui::PushStyleColor(ImGuiCol_FrameBg, fieldColor);
    ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    float iconSize = fieldHeight * 0.6f;
    float iconPaddingLeft = 8.0f;
    float textPaddingLeft = iconPaddingLeft + iconSize + 4.0f;

    float fontSize = ImGui::GetFontSize();
    float newPaddingY = std::max(0.0f, (fieldHeight - fontSize) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(textPaddingLeft, newPaddingY));
    ImGui::PushItemWidth(searchWidth);
    ImGui::InputTextWithHint("##search", "Search by name or PID", searchBuffer, IM_ARRAYSIZE(searchBuffer));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    {
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        ImVec2 iconPos(rectMin.x + iconPaddingLeft, rectMin.y + (rectMax.y - rectMin.y - iconSize) * 0.5f);
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddImage((void*)(intptr_t)searchIcon.getNativeHandle(),
                           iconPos,
                           ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
    }
    
    ImGui::SameLine(0, spacing);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(textPaddingLeft, newPaddingY));
    ImGui::PushItemWidth(comboWidth);
    ImGui::Combo("##sort", &sortIndex, sortOptions, IM_ARRAYSIZE(sortOptions));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    {
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        ImVec2 iconPos(rectMin.x + iconPaddingLeft, rectMin.y + (rectMax.y - rectMin.y - iconSize) * 0.5f);
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddImage((void*)(intptr_t)sortIcon.getNativeHandle(),
                           iconPos,
                           ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
    }

    ImGui::SameLine(0, spacing);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(textPaddingLeft, newPaddingY));
    ImGui::PushItemWidth(comboWidth);
    ImGui::Combo("##status", &statusIndex, statusOptions, IM_ARRAYSIZE(statusOptions));
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    {
        ImVec2 rectMin = ImGui::GetItemRectMin();
        ImVec2 rectMax = ImGui::GetItemRectMax();
        ImVec2 iconPos(rectMin.x + iconPaddingLeft, rectMin.y + (rectMax.y - rectMin.y - iconSize) * 0.5f);
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddImage((void*)(intptr_t)statusIcon.getNativeHandle(),
                           iconPos,
                           ImVec2(iconPos.x + iconSize, iconPos.y + iconSize));
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::End();
    ImGui::PopFont();
}


void RenderProcessList(ImFont* font) {
    static int selected_row = -1;
    static std::vector<ProcessInfo> all_processes;
    static std::map<int, ProcessInfo> prev_process_map;
    static CpuStats prev_cpu_stats = getCpuStats();
    static double last_update_time = -2.0;

    double current_time = ImGui::GetTime();
    if (current_time - last_update_time > 2.0) {
        
        CpuStats current_cpu_stats = getCpuStats();
        long long total_jiffies_delta = current_cpu_stats.Total() - prev_cpu_stats.Total();
        if (total_jiffies_delta == 0) total_jiffies_delta = 1;

        std::vector<ProcessInfo> new_processes = FetchRealProcessData();
        std::map<int, ProcessInfo> new_process_map;

        for(auto& p : new_processes) {
            auto it = prev_process_map.find(p.pid);
            if (it != prev_process_map.end()) {
                unsigned long long jiffies_delta = p.active_jiffies - it->second.active_jiffies;
                p.cpu = (100.0f * (float)jiffies_delta) / (float)total_jiffies_delta;
            }
            new_process_map[p.pid] = p;
        }
        
        all_processes = new_processes;
        prev_process_map = new_process_map;
        prev_cpu_stats = current_cpu_stats;
        last_update_time = current_time;
    }

    std::vector<ProcessInfo> filtered_processes;
    std::string search_lower = searchBuffer;
    std::transform(search_lower.begin(), search_lower.end(), search_lower.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    std::string selected_status = statusOptions[statusIndex];

    for (const auto& process : all_processes) {
        
        if (selected_status != "All") {
            std::string status_char = "";
            if (!process.status.empty()) {
                size_t first_char_pos = process.status.find_first_not_of(" \n\r\t");
                if (first_char_pos != std::string::npos) {
                    status_char = process.status.substr(first_char_pos, 1);
                }
            }

            if (selected_status == "Running" && status_char != "R") {
                continue;
            }
            if (selected_status == "Sleeping" && status_char != "S") {
                continue;
            }
            if (selected_status == "Stopped" && status_char != "T") {
                continue;
            }
        }

        if (!search_lower.empty()) {
            std::string name_lower = process.name;
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(),
                           [](unsigned char c){ return std::tolower(c); });
            std::string pid_str = std::to_string(process.pid);

            if (name_lower.find(search_lower) == std::string::npos &&
                pid_str.find(search_lower) == std::string::npos) {
                continue;
            }
        }
        filtered_processes.push_back(process);
    }

    std::string sort_by = sortOptions[sortIndex];
    std::sort(filtered_processes.begin(), filtered_processes.end(), 
        [&](const ProcessInfo& a, const ProcessInfo& b) {
            if (sort_by == "CPU %") { return a.cpu > b.cpu; }
            if (sort_by == "Memory %") { return a.memory > b.memory; }
            if (sort_by == "PID") { return a.pid < b.pid; }
            if (sort_by == "Name") { 
                return std::lexicographical_compare(
                    a.name.begin(), a.name.end(),
                    b.name.begin(), b.name.end(),
                    [](unsigned char c1, unsigned char c2) { return std::tolower(c1) < std::tolower(c2); }
                );
            }
            return false;
    });

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float headerHeight = std::clamp(displaySize.y * 0.06f, 40.0f, 80.0f);
    float headerEndY = (displaySize.y / 2) + headerHeight;
    float listStartY = headerEndY;
    float listHeight = displaySize.y - listStartY;

    ImGui::SetNextWindowPos(ImVec2(0.0f, listStartY));
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, listHeight));
    
    ImGui::PushFont(font);
     
    if (ImGui::Begin("Process List", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
    {
        ImGuiTableFlags flags = ImGuiTableFlags_Borders |
                                ImGuiTableFlags_RowBg |
                                ImGuiTableFlags_ScrollY |
                                ImGuiTableFlags_Resizable |
                                ImGuiTableFlags_SizingStretchSame;

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.0f, 8.0f));
        ImGui::SetCursorPosX(20.0f);
        float tableWidth = displaySize.x - 40.0f;

        // Changed column count from 5 to 6
        if (ImGui::BeginTable("ProcessTable", 6, flags, ImVec2(tableWidth, -1.0f)))
        {
            ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("CPU (%)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Memory (MB)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            // Added 6th column for the Kill button
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 60.0f); 

            ImGui::TableHeadersRow();

            for (size_t i = 0; i < filtered_processes.size(); i++)
            {
                ImGui::TableNextRow();

                if (i % 2 == 0)
                {
                    // FIXED: Removed redundant ImGui::GetColorU32() wrapper
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(23, 43, 58, 255)); 
                }
                else
                {
                    // FIXED: Removed redundant ImGui::GetColorU32() wrapper
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(14, 20, 24, 255)); 
                }

                ImGui::TableNextColumn();
                char label[32];
                sprintf(label, "%d", filtered_processes[i].pid);
                ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                if (ImGui::Selectable(label, selected_row == (int)i, selectable_flags))
                {
                    selected_row = i;
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", filtered_processes[i].name.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%.1f", filtered_processes[i].cpu);

                ImGui::TableNextColumn();
                ImGui::Text("%.1f", filtered_processes[i].memory);

                ImGui::TableNextColumn();
                ImGui::Text("%s", filtered_processes[i].status.c_str());

                 
                ImGui::TableNextColumn();
                
               ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(190, 50, 50, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(210, 60, 60, 255));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(230, 70, 70, 255));
                
                int pid_to_kill = filtered_processes[i].pid;
                char button_label[32];
                sprintf(button_label, "Kill##%d", pid_to_kill);

                
                bool is_self = (pid_to_kill == getpid());
                if (is_self) {
                    
                    ImGui::BeginDisabled();
                }

                 
                if (ImGui::Button(button_label, ImVec2(-1, 0))) {
                    if (!is_self) {
                        kill(pid_to_kill, SIGKILL);  
                    }
                }

                if (is_self) {
                    
                    ImGui::EndDisabled();
                }

                ImGui::PopStyleColor(3);  
                  
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
        ImGui::End();
    }
    
    ImGui::PopFont();
}