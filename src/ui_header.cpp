#include "ui_header.hpp"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <time.h>
namespace fs = std::filesystem;

float getAverageLoadOfSystem(){
    double load[1];

    if (getloadavg(load, 1) == -1) {
        std::cerr << "Error: Could not retrieve load average." << std::endl;
        return 0.0f;
    }
    return load[0];
};

unsigned int getCoreCount() {
    return std::thread::hardware_concurrency();
}


bool RenderHeaderBar(sf::Texture& logoTexture,
                     sf::Texture& chipIcon1, sf::Texture& chipIcon2,
                     sf::Texture& clockIcon, sf::Texture& refreshIcon,
                     ImFont* font) {
    bool refreshed = false;

    float avgLoadOfSystem = getAverageLoadOfSystem();
    unsigned int coreCount = getCoreCount();
     
    const char* statusText;
    ImU32 statusColor;

    if (avgLoadOfSystem > (float)coreCount * 1.5f) {
        statusText = "Critical";
        statusColor = IM_COL32(220, 30, 30, 220);
    } else if (avgLoadOfSystem > (float)coreCount * 0.75f) {
        statusText = "High Load";
        statusColor = IM_COL32(255, 165, 0, 220);
    } else {
        statusText = "Healthy";
        statusColor = IM_COL32(31, 185, 122, 255);
    }

    ImGui::PushFont(font);
    ImVec2 display = ImGui::GetIO().DisplaySize;
    float windowHeight = std::clamp(display.y * 0.06f, 40.0f, 80.0f);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(display.x, windowHeight));
    ImGui::Begin("Header", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 p_min = ImGui::GetWindowPos();
    ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowSize().x, p_min.y + ImGui::GetWindowSize().y);
    draw_list->AddRectFilled(p_min, p_max, IM_COL32(14, 20, 24, 255));

    float logoSize = std::clamp(display.y * 0.035f, 20.0f, 36.0f);
    float y_center_padding = (windowHeight - logoSize) * 0.5f;

    ImGui::SetCursorPos(ImVec2(10, y_center_padding));
    ImGui::Image((void*)(intptr_t)logoTexture.getNativeHandle(), ImVec2(logoSize, logoSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(10 + logoSize + 6, (windowHeight - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::Text("System Monitor Tool");
    ImGui::SameLine();

    ImVec2 textSize = ImGui::CalcTextSize("System Monitor Tool");
    float chipWidth = std::clamp(display.x * 0.12f, 90.0f, 220.0f);
    float chipHeight = std::clamp(display.y * 0.04f, 28.0f, 40.0f);
    float chipY = (windowHeight - chipHeight) * 0.5f;
    float chipX1 = 10 + logoSize + 6 + textSize.x + 12.0f;
    float chipGap = std::clamp(display.x * 0.01f, 8.0f, 20.0f);

    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX1, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX1 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        statusColor, chipHeight / 2.0f);
    float iconSize = std::clamp(chipHeight * 0.6f, 14.0f, 24.0f);
    float iconY = chipY + (chipHeight - iconSize) / 2.0f;
    float textY = chipY + (chipHeight - ImGui::GetTextLineHeight()) / 2.0f;
    ImGui::SetCursorPos(ImVec2(chipX1 + 8, iconY));
    ImGui::Image((void*)(intptr_t)chipIcon1.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX1 + 8 + iconSize + 8, textY));
    ImGui::TextColored(ImVec4(1,1,1,1), "%s", statusText);

    float chipX2 = chipX1 + chipWidth + chipGap;
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX2, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX2 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(204, 102, 0, 255), chipHeight / 2.0f);
    ImGui::SetCursorPos(ImVec2(chipX2 + 8, iconY));
    ImGui::Image((void*)(intptr_t)chipIcon2.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX2 + 8 + iconSize + 8, textY));
    ImGui::TextColored(ImVec4(1,1,1,1), "Load: %.2f", avgLoadOfSystem);


    static char timeBuf[64];
    time_t now = time(0);
    tm* ltm = localtime(&now);
    strftime(timeBuf, sizeof(timeBuf), "%I:%M:%S %p", ltm);

    float buttonSize = std::clamp(chipHeight * 0.9f, 24.0f, 36.0f);
    float timeWidth = ImGui::CalcTextSize(timeBuf).x + iconSize + 16.0f;
    float buttonWidth = buttonSize + 16.0f;

    float clockStartX = display.x - buttonWidth - timeWidth;
    
    ImGui::SetCursorPos(ImVec2(clockStartX, iconY));
    ImGui::Image((void*)(intptr_t)clockIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(clockStartX + iconSize + 8, textY));
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", timeBuf);


    ImGui::SetCursorPos(ImVec2(display.x - buttonSize - 15.0f, (windowHeight - buttonSize) * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(50, 80, 100, 150));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(70, 100, 120, 200));
    if (ImGui::ImageButton("##refresh", (void*)(intptr_t)refreshIcon.getNativeHandle(), ImVec2(buttonSize, buttonSize))) { 
        refreshed = true; 
    }
    ImGui::PopStyleColor(3);


    ImVec2 p1 = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + windowHeight - 1.0f);
    ImVec2 p2 = ImVec2(ImGui::GetWindowPos().x + display.x, ImGui::GetWindowPos().y + windowHeight - 1.0f);
    draw_list->AddLine(p1, p2, IM_COL32(23, 43, 58, 255), 4.0f);
    
    ImGui::PopFont();
    ImGui::End();
    return refreshed;
}