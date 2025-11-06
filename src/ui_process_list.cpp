#include "ui_process_list.hpp"
#include <iostream>
#include <vector>
#include <string>

struct ProcessInfo {
    int pid;
    std::string name;
    float cpu;
    float memory;
    std::string status;
};

std::vector<ProcessInfo> processes = {
    {101, "chrome.exe", 12.5f, 250.8f, "Running"},
    {205, "code.exe", 5.2f, 180.2f, "Running"},
    {300, "your_app.exe", 45.1f, 800.0f, "Running"},
    {404, "svchost.exe", 0.1f, 15.5f, "Suspended"},
    {500, "explorer.exe", 1.8f, 95.1f, "Running"},
    {501, "taskmgr.exe", 2.3f, 45.6f, "Running"},
    {600, "spotify.exe", 3.1f, 120.7f, "Running"}
};

void RenderProcessList(ImFont* font) {
    static int selected_row = -1;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(ImVec2(displaySize.x, displaySize.y));
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

        if (ImGui::BeginTable("ProcessTable", 5, flags, ImVec2(tableWidth, -1.0f)))
        {
            ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("CPU (%)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Memory (MB)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);

            ImGui::TableHeadersRow();

            for (int i = 0; i < processes.size(); i++)
            {
                ImGui::TableNextRow();

                if (i % 2 == 0)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(IM_COL32(23, 43, 58, 255))); 
                }
                else
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(IM_COL32(14, 20, 24, 255))); 
                }

                ImGui::TableNextColumn();
                char label[32];
                sprintf(label, "%d", processes[i].pid);
                ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                if (ImGui::Selectable(label, selected_row == i, selectable_flags))
                {
                    selected_row = i;
                    std::cout << "Selected Process: " << processes[i].name << std::endl;
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", processes[i].name.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%.1f", processes[i].cpu);

                ImGui::TableNextColumn();
                ImGui::Text("%.1f", processes[i].memory);

                ImGui::TableNextColumn();
                ImGui::Text("%s", processes[i].status.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
        ImGui::End();
    }
    
    ImGui::PopFont();
}