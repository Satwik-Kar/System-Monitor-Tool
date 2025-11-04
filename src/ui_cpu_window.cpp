#include "ui_cpu_window.hpp"
#include <vector>
#include <cstdlib>

void RenderCPUWindow(sf::Texture& cpuTexture,
                     sf::Texture& speedIcon,
                     sf::Texture& clockIcon,
                     sf::Texture& userIcon,
                     sf::Texture& systemIcon,
                     sf::Texture& idleIcon,
                     ImFont* font) {
    ImGui::PushFont(font);
    ImGui::SetNextWindowPos(ImVec2(20, 60), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 350));
    ImGui::Begin("CPU Usage", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar);

    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Image((void*)(intptr_t)cpuTexture.getNativeHandle(), ImVec2(28, 28));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
    ImGui::TextColored(ImVec4(255, 255, 255, 255), "CPU Usage");

    float dotSize = 9.0f;
    float spacing = 60.0f;
    ImVec2 winPos = ImGui::GetWindowPos();
    float baseY = winPos.y + 18.0f;
    float startX = ImGui::GetWindowContentRegionMax().x - 200;

    ImDrawList* drawListDots = ImGui::GetWindowDrawList();
    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX, baseY), dotSize / 2, IM_COL32(0, 200, 200, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + 12, baseY - 8));
    ImGui::TextColored(ImVec4(0, 1, 1, 1), "User");

    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX + spacing, baseY), dotSize / 2, IM_COL32(120, 180, 255, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + spacing + 12, baseY - 8));
    ImGui::TextColored(ImVec4(0.7, 0.9, 1, 1), "System");

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 8));

    ImDrawList* chipDrawList = ImGui::GetWindowDrawList();
    ImVec2 chipPos = ImGui::GetCursorScreenPos();
    DrawChip(chipDrawList, chipPos, 10, 10, 150, 36, IM_COL32(23, 43, 58, 255), speedIcon, "Avg 58%%", ImVec4(0.3, 0.6, 0.8, 1));
    DrawChip(chipDrawList, chipPos, 180, 10, 150, 36, IM_COL32(28, 55, 70, 255), clockIcon, "3.6 GHz", ImVec4(0.4, 0.8, 1, 1));

    ImGui::Dummy(ImVec2(0, 16));

    static float userCPU[100] = {};
    static float systemCPU[100] = {};
    static int dataIndex = 0;
    static float elapsed = 0.0f;
    const float UPDATE_INTERVAL = 0.2f;

    elapsed += ImGui::GetIO().DeltaTime;
    if (elapsed >= UPDATE_INTERVAL) {
        elapsed = 0.0f;
        userCPU[dataIndex] = 30.0f + static_cast<float>(rand() % 70);
        systemCPU[dataIndex] = 20.0f + static_cast<float>(rand() % 50);
        dataIndex = (dataIndex + 1) % IM_ARRAYSIZE(userCPU);
    }

    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, 120);
    ImVec2 graphPos = ImGui::GetCursorScreenPos();
    ImDrawList* graph = ImGui::GetWindowDrawList();
    float cornerRadius = 14.0f;
    ImU32 bgColor = IM_COL32(15, 25, 35, 255);
    ImU32 borderColor = IM_COL32(35, 55, 75, 255);
    graph->AddRectFilled(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), bgColor, cornerRadius);
    graph->AddRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), borderColor, cornerRadius, 0, 2.0f);

    for (int i = 1; i < IM_ARRAYSIZE(userCPU); i++) {
        int i0 = (dataIndex + i - 1) % IM_ARRAYSIZE(userCPU);
        int i1 = (dataIndex + i) % IM_ARRAYSIZE(userCPU);
        float x0 = graphPos.x + ((i - 1) / 99.0f) * graphSize.x;
        float x1 = graphPos.x + (i / 99.0f) * graphSize.x;
        float y0u = graphPos.y + graphSize.y * (1.0f - userCPU[i0] / 100.0f);
        float y1u = graphPos.y + graphSize.y * (1.0f - userCPU[i1] / 100.0f);
        float y0s = graphPos.y + graphSize.y * (1.0f - systemCPU[i0] / 100.0f);
        float y1s = graphPos.y + graphSize.y * (1.0f - systemCPU[i1] / 100.0f);
        graph->AddLine(ImVec2(x0, y0u), ImVec2(x1, y1u), IM_COL32(0, 200, 200, 255), 2.0f);
        graph->AddLine(ImVec2(x0, y0s), ImVec2(x1, y1s), IM_COL32(120, 180, 255, 255), 2.0f);
    }

    ImGui::Dummy(graphSize);
    ImGui::Dummy(ImVec2(0, 20));

    ImDrawList* chips = ImGui::GetWindowDrawList();
    ImVec2 chipBase = ImGui::GetCursorScreenPos();
    struct ChipInfo {
        sf::Texture* icon;
        const char* label;
        ImU32 color;
        ImVec4 textColor;
    };
    std::vector<ChipInfo> list = {
        { &userIcon, "User: 12%%", IM_COL32(23,43,58,255), ImVec4(0.6,0.9,1,1) },
        { &systemIcon, "System: 30%%", IM_COL32(28,55,70,255), ImVec4(0.4,0.8,1,1) },
        { &idleIcon, "Idle: 48%%", IM_COL32(40,70,90,255), ImVec4(0.5,0.8,1,1) },
        { &cpuTexture, "Available Cores: 16", IM_COL32(50,90,110,255), ImVec4(0.7,1,0.9,1) }
    };

    float xOffset = 10, chipSpacing = 20, paddingX = 40, height = 56;
    for (const auto& c : list) {
        float textWidth = ImGui::CalcTextSize(c.label).x;
        float width = textWidth + paddingX + 40.0f;
        DrawChip(chips, chipBase, xOffset, 0, width, height, c.color, *c.icon, c.label, c.textColor);
        xOffset += width + chipSpacing;
    }

    ImGui::End();
    ImGui::PopFont();
}
