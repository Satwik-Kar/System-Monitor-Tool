#include "ui_memory_window.hpp"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdio>

struct MemoryInfo {
    long memTotal = 0;
    long memAvailable = 0;
    long memCached = 0;
    long buffers = 0;
    long swapTotal = 0;
    long swapFree = 0;

    long memUsed = 0;
    long memCachedAndBuffers = 0;
    long swapUsed = 0;

    float memUsedPercent = 0.0f;
    float memCachedPercent = 0.0f;
    float memAvailablePercent = 0.0f;
    float swapUsedPercent = 0.0f;
};

bool GetMemoryInfo(MemoryInfo& info) {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::map<std::string, long> memData;
    while (std::getline(file, line)) {
        std::string key;
        long value;
        std::string unit;
        std::stringstream ss(line);
        ss >> key >> value >> unit;
        if (!key.empty()) {
            key.pop_back();
            memData[key] = value;
        }
    }
    file.close();

    info.memTotal = memData["MemTotal"];
    info.memAvailable = memData["MemAvailable"];
    info.memCached = memData["Cached"];
    info.buffers = memData["Buffers"];
    info.swapTotal = memData["SwapTotal"];
    info.swapFree = memData["SwapFree"];
    info.memCachedAndBuffers = info.memCached + info.buffers;

    if (info.memTotal > 0) {
        info.memUsed = info.memTotal - info.memAvailable;
        info.memUsedPercent = (static_cast<float>(info.memUsed) / info.memTotal) * 100.0f;
        info.memCachedPercent = (static_cast<float>(info.memCachedAndBuffers) / info.memTotal) * 100.0f;
        info.memAvailablePercent = (static_cast<float>(info.memAvailable) / info.memTotal) * 100.0f;
    }

    if (info.swapTotal > 0) {
        info.swapUsed = info.swapTotal - info.swapFree;
        info.swapUsedPercent = (static_cast<float>(info.swapUsed) / info.swapTotal) * 100.0f;
    } else {
        info.swapUsed = 0;
        info.swapUsedPercent = 0.0f;
    }
    
    return true;
}

void RenderMemoryWindow(sf::Texture& memoryTexture,
                     sf::Texture& diskIcon,
                     sf::Texture& swapIcon,
                     sf::Texture& usedIcon,
                     sf::Texture& cachedIcon,
                     sf::Texture& freeIcon,
                     ImFont* font) {
    ImGui::PushFont(font);
    ImVec2 display = ImGui::GetIO().DisplaySize;
    if (display.x < 400.0f || display.y < 300.0f) {
        ImGui::PopFont();
        return;
    }

    float padding = std::clamp(display.x * 0.02f, 10.0f, 40.0f);
    float winW = std::max(220.0f, display.x * 0.48f - padding);
    float winH = std::max(250.0f, display.y * 0.35f);
    ImGui::SetNextWindowPos(ImVec2(display.x * 0.5f + padding * 0.5f, 80), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(winW, winH));
    ImGui::Begin("Memory Usage", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetCursorPos(ImVec2(10, 10));
    ImGui::Image((void*)(intptr_t)memoryTexture.getNativeHandle(), ImVec2(std::clamp(winH * 0.08f, 20.0f, 40.0f), std::clamp(winH * 0.08f, 20.0f, 40.0f)));
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
    ImGui::TextColored(ImVec4(1,1,1,1), "Memory Usage");

    float dotSize = std::max(6.0f, winH * 0.05f);
    float spacing = std::clamp(winW * 0.12f, 40.0f, 120.0f);
    ImVec2 winPos = ImGui::GetWindowPos();
    float baseY = winPos.y + 18.0f;
    float startX = ImGui::GetWindowContentRegionMax().x - std::clamp(winW * 0.25f, 80.0f, 220.0f);

    ImDrawList* drawListDots = ImGui::GetWindowDrawList();
    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX, baseY), dotSize / 2, IM_COL32(0, 200, 200, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0,1,1,1), "Used");

    drawListDots->AddCircleFilled(ImVec2(winPos.x + startX + spacing, baseY), dotSize / 2, IM_COL32(120, 180, 255, 255));
    ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + spacing + dotSize + 6, baseY - dotSize * 0.5f));
    ImGui::TextColored(ImVec4(0.7,0.9,1,1), "Cached");

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 8));

    static MemoryInfo memInfo;
    static float usedMemoryHistory[100] = {};
    static float cachedMemoryHistory[100] = {};
    static int dataIndex = 0;
    static float dataElapsed = 1.0f; 
    const float UPDATE_INTERVAL = 1.0f;

    static char memChipBuffer[64] = "0.0 GB / 0.0 GB";
    static char swapChipBuffer[64] = "Swap: 0.0 GB";
    static char usedLabel[32] = "Used: 0%";
    static char cachedLabel[32] = "Cached: 0%";
    static char freeLabel[32] = "Free: 0%";
    static char swappedLabel[32] = "Swapped: 0%";

    dataElapsed += ImGui::GetIO().DeltaTime;
    if (dataElapsed >= UPDATE_INTERVAL) {
        dataElapsed = 0.0f;
        
        if (GetMemoryInfo(memInfo)) {
            usedMemoryHistory[dataIndex] = memInfo.memUsedPercent;
            cachedMemoryHistory[dataIndex] = memInfo.memCachedPercent;

            float memUsedGB = memInfo.memUsed / (1024.0f * 1024.0f);
            float memTotalGB = memInfo.memTotal / (1024.0f * 1024.0f);
            std::snprintf(memChipBuffer, 64, "%.1f GB / %.1f GB", memUsedGB, memTotalGB);

            float swapUsedGB = memInfo.swapUsed / (1024.0f * 1024.0f);
            std::snprintf(swapChipBuffer, 64, "Swap: %.1f GB", swapUsedGB);

            std::snprintf(usedLabel, 32, "Used: %.0f%%", memInfo.memUsedPercent);
            std::snprintf(cachedLabel, 32, "Cached: %.0f%%", memInfo.memCachedPercent);
            std::snprintf(freeLabel, 32, "Free: %.0f%%", memInfo.memAvailablePercent);
            std::snprintf(swappedLabel, 32, "Swapped: %.0f%%", memInfo.swapUsedPercent);
        }
        
        dataIndex = (dataIndex + 1) % IM_ARRAYSIZE(usedMemoryHistory);
    }

    ImDrawList* chipDrawList = ImGui::GetWindowDrawList();
    ImVec2 chipPos = ImGui::GetCursorScreenPos();
    float chipHeight = std::max(36.0f, winH * 0.09f);
    float chip1Width = std::clamp(winW * 0.36f, 140.0f, 320.0f);
    float chip2Width = std::clamp(winW * 0.28f, 100.0f, 220.0f);
    DrawChip(chipDrawList, chipPos, 10, 10, chip1Width, chipHeight, IM_COL32(23, 43, 58, 255), diskIcon, memChipBuffer, ImVec4(0.3, 0.6, 0.8, 1));
    DrawChip(chipDrawList, chipPos, 20 + chip1Width, 10, chip2Width, chipHeight, IM_COL32(28, 55, 70, 255), swapIcon, swapChipBuffer, ImVec4(0.4, 0.8, 1, 1));

    ImGui::Dummy(ImVec2(0, std::clamp(winH * 0.02f, 8.0f, 24.0f)));

    ImVec2 graphSize(ImGui::GetContentRegionAvail().x, std::max( (float)100.0f, ImGui::GetContentRegionAvail().y * 0.28f ));
    ImVec2 graphPos = ImGui::GetCursorScreenPos();
    ImDrawList* graph = ImGui::GetWindowDrawList();
    float cornerRadius = std::max(8.0f, graphSize.y * 0.12f);
    ImU32 bgColor = IM_COL32(15, 25, 35, 255);
    ImU32 borderColor = IM_COL32(35, 55, 75, 255);
    graph->AddRectFilled(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), bgColor, cornerRadius);
    graph->AddRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), borderColor, cornerRadius, 0, 2.0f);

    graph->PushClipRect(graphPos, ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y), true);
    for (int i = 1; i < IM_ARRAYSIZE(usedMemoryHistory); i++) {
        int i0 = (dataIndex + i - 1) % IM_ARRAYSIZE(usedMemoryHistory);
        int i1 = (dataIndex + i) % IM_ARRAYSIZE(usedMemoryHistory);
        float x0 = graphPos.x + ((i - 1) / 99.0f) * graphSize.x;
        float x1 = graphPos.x + (i / 99.0f) * graphSize.x;
        float y0u = graphPos.y + graphSize.y * (1.0f - usedMemoryHistory[i0] / 100.0f);
        float y1u = graphPos.y + graphSize.y * (1.0f - usedMemoryHistory[i1] / 100.0f);
        float y0s = graphPos.y + graphSize.y * (1.0f - cachedMemoryHistory[i0] / 100.0f);
        float y1s = graphPos.y + graphSize.y * (1.0f - cachedMemoryHistory[i1] / 100.0f);
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
        { &usedIcon, usedLabel, IM_COL32(23,43,58,255), ImVec4(0.6,0.9,1,1) },
        { &freeIcon, freeLabel, IM_COL32(40,70,90,255), ImVec4(0.5,0.8,1,1) },
        { &cachedIcon, cachedLabel, IM_COL32(28,55,70,255), ImVec4(0.4,0.8,1,1) },
        { &freeIcon, swappedLabel, IM_COL32(50,90,110,255), ImVec4(0.7,1,0.9,1) }
    };

    float contentWidth = ImGui::GetContentRegionAvail().x;
    float xOffset = std::max(8.0f, contentWidth * 0.02f);
    float chipSpacing = std::max(8.0f, contentWidth * 0.015f);
    float height2 = std::max(40.0f, contentWidth * 0.06f);
    float paddingX = std::clamp(contentWidth * 0.05f, 24.0f, 80.0f);
    for (const auto& c : list) {
        float textWidth = ImGui::CalcTextSize(c.label).x;
        float width = std::min(contentWidth * 0.9f, textWidth + paddingX + std::max(36.0f, contentWidth * 0.06f));
        DrawChip(chips, chipBase, xOffset, 0, width, height2, c.color, *c.icon, c.label, c.textColor);
        xOffset += width + chipSpacing;
        if (xOffset > contentWidth) break;
    }

    ImGui::End();
    ImGui::PopFont();       
}