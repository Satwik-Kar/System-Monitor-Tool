#include "ui_cpu_window.hpp"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>

void RenderCPUWindow(sf::Texture& cpuTexture,
                     sf::Texture& speedIcon,
                     sf::Texture& clockIcon,
                     sf::Texture& userIcon,
                     sf::Texture& systemIcon,
                     sf::Texture& idleIcon,
                     ImFont* font) {
    ImGui::PushFont(font);
    ImVec2 display = ImGui::GetIO().DisplaySize;
    if (display.x < 400.0f || display.y < 300.0f) {
        ImGui::PopFont();
        return;
    }

    float paddingCpu = std::clamp(display.x * 0.02f, 10.0f, 40.0f);
    float winW = std::max(220.0f, display.x * 0.48f - paddingCpu);
    float winH = std::max(250.0f, display.y * 0.35f);
    ImGui::SetNextWindowPos(ImVec2(paddingCpu, 60), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::Begin("CPU Usage", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Image((void*)(intptr_t)cpuTexture.getNativeHandle(), ImVec2(std::clamp(winH * 0.08f, 20.0f, 40.0f), std::clamp(winH * 0.08f, 20.0f, 40.0f)));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
    ImGui::TextColored(ImVec4(1,1,1,1), "CPU Usage");

    float dotSize = std::max(6.0f, winH * 0.05f);
    float spacing = std::clamp(winW * 0.12f, 40.0f, 120.0f);
    ImVec2 winPos = ImGui::GetWindowPos();
    float baseY = winPos.y + 18.0f;
    float startX = ImGui::GetWindowContentRegionMax().x - std::clamp(winW * 0.25f, 80.0f, 220.0f);

    ImDrawList* drawListDots = ImGui::GetWindowDrawList();
    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX, baseY), dotSize / 2, IM_COL32(0, 200, 200, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0,1,1,1), "User");

    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX + spacing, baseY), dotSize / 2, IM_COL32(120, 180, 255, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + spacing + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0.7,0.9,1,1), "System");

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 8));

    ImDrawList* chipDrawList = ImGui::GetWindowDrawList();
    ImVec2 chipPos = ImGui::GetCursorScreenPos();
    float chipHeight = std::max(36.0f, winH * 0.09f);
    float chipPaddingX = std::clamp(winW * 0.05f, 24.0f, 60.0f);
    float chip1Width = std::clamp(winW * 0.32f, 120.0f, 260.0f);
    float chip2Width = std::clamp(winW * 0.28f, 100.0f, 220.0f);
    DrawChip(chipDrawList, chipPos, 10, 10, chip1Width, chipHeight, IM_COL32(23, 43, 58, 255), speedIcon, "Avg 58%%", ImVec4(0.3, 0.6, 0.8, 1));
    DrawChip(chipDrawList, chipPos, 20 + chip1Width, 10, chip2Width, chipHeight, IM_COL32(28, 55, 70, 255), clockIcon, "3.6 GHz", ImVec4(0.4, 0.8, 1, 1));

    ImGui::Dummy(ImVec2(0, std::clamp(winH * 0.02f, 8.0f, 24.0f)));

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

    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, std::max( (float)100.0f, ImGui::GetContentRegionAvail().y * 0.28f ));
    ImVec2 graphPos = ImGui::GetCursorScreenPos();
    ImDrawList* graph = ImGui::GetWindowDrawList();
    float cornerRadius = std::max(8.0f, graphSize.y * 0.12f);
    ImU32 bgColor = IM_COL32(15, 25, 35, 255);
    ImU32 borderColor = IM_COL32(35, 55, 75, 255);
    graph->AddRectFilled(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), bgColor, cornerRadius);
    graph->AddRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), borderColor, cornerRadius, 0, 2.0f);

    graph->PushClipRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), true);
    for (int i = 1; i < IM_ARRAYSIZE(userCPU); i++) {
        int i0 = (dataIndex + i - 1) % IM_ARRAYSIZE(userCPU);
        int i1 = (dataIndex + i) % IM_ARRAYSIZE(userCPU);
        float x0 = graphPos.x + ((i - 1) / 99.0f) * graphSize.x;
        float x1 = graphPos.x + (i / 99.0f) * graphSize.x;
        float y0u = graphPos.y + graphSize.y * (1.0f - userCPU[i0] / 100.0f);
        float y1u = graphPos.y + graphSize.y * (1.0f - userCPU[i1] / 100.0f);
        float y0s = graphPos.y + graphSize.y * (1.0f - systemCPU[i0] / 100.0f);
        float y1s = graphPos.y + graphSize.y * (1.0f - systemCPU[i1] / 100.0f);
        graph->AddLine(ImVec2(x0, y0u), ImVec2(x1, y1u), IM_COL32(0, 200, 200, 255), std::max(1.0f, graphSize.y * 0.012f));
        graph->AddLine(ImVec2(x0, y0s), ImVec2(x1, y1s), IM_COL32(120, 180, 255, 255), std::max(1.0f, graphSize.y * 0.012f));
    }
    graph->PopClipRect();

    ImGui::Dummy(graphSize);
    ImGui::Dummy(ImVec2(0, std::clamp(winH * 0.04f, 12.0f, 28.0f)));

    ImDrawList* chips = ImGui::GetWindowDrawList();
    ImVec2 chipBase = ImGui::GetCursorScreenPos();
    struct ChipInfo {
        sf::Texture* icon;
        const char* label;
        ImU32 color;
        ImVec4 textColor;
    };
    std::vector<ChipInfo> list = {
        { &userIcon, "User: 12%", IM_COL32(23,43,58,255), ImVec4(0.6,0.9,1,1) },
        { &systemIcon, "System: 30%", IM_COL32(28,55,70,255), ImVec4(0.4,0.8,1,1) },
        { &idleIcon, "Idle: 48%", IM_COL32(40,70,90,255), ImVec4(0.5,0.8,1,1) },
        { &cpuTexture, "Available Cores: 16", IM_COL32(50,90,110,255), ImVec4(0.7,1,0.9,1) }
    };

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float xOffset = std::max(8.0f, contentWidth * 0.02f);
    float chipSpacing = std::max(8.0f, contentWidth * 0.015f);
    float height = std::max(40.0f, contentWidth * 0.06f);
    float paddingX = std::clamp(contentWidth * 0.05f, 24.0f, 80.0f);
    for (const auto& c : list) {
        float textWidth = ImGui::CalcTextSize(c.label).x;
        float width = std::min(contentWidth * 0.9f, textWidth + paddingX + std::max(36.0f, contentWidth * 0.06f));
        DrawChip(chips, chipBase, xOffset, 0, width, height, c.color, *c.icon, c.label, c.textColor);
        xOffset += width + chipSpacing;
        if (xOffset > contentWidth) break;
    }

    ImGui::End();
    ImGui::PopFont();
}
