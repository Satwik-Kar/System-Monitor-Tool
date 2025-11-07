#include "ui_process_list.hpp"
#include "process_info.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void RenderProcessList(ImFont* font) {
    static int selected_row = -1;
    static std::vector<ProcessInfo> processes;
    static double last_update_time = -2.0;

    double current_time = ImGui::GetTime();
    if (current_time - last_update_time > 2.0) {
        processes = FetchRealProcessData();
        last_update_time = current_time;
    }

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