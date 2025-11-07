#include "ui_header.hpp"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

float getAverageLoadOfSystem(){
    double load[1];

    if (getloadavg(load, 1) == -1) {
        std::cerr << "Error: Could not retrieve load average." << std::endl;
        return 0.0f;
    }
    return load[0];


};



bool RenderHeaderBar(sf::Texture& logoTexture,
                     sf::Texture& chipIcon1, sf::Texture& chipIcon2,
                     sf::Texture& clockIcon, sf::Texture& refreshIcon,
                     ImFont* font) {
    bool refreshed = false;

    float avgLoadOfSystem = getAverageLoadOfSystem();
     

    ImGui::PushFont(font);
    ImVec2 display = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(display.x, std::clamp(display.y * 0.06f, 40.0f, 80.0f)));
    ImGui::Begin("Header", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 p_min = ImGui::GetWindowPos();
    ImVec2 p_max = ImVec2(p_min.x + ImGui::GetWindowSize().x, p_min.y + ImGui::GetWindowSize().y);
    draw_list->AddRectFilled(p_min, p_max, IM_COL32(14, 20, 24, 255));

    ImGui::SetCursorPos(ImVec2(10, 9));
    float logoSize = std::clamp(display.y * 0.035f, 20.0f, 36.0f);
    ImGui::Image((void*)(intptr_t)logoTexture.getNativeHandle(), ImVec2(logoSize, logoSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(10 + logoSize + 6, 15));
    ImGui::Text("System Monitor Tool");
    ImGui::SameLine();

    ImVec2 textSize = ImGui::CalcTextSize("System Monitor Tool");
    float chipWidth = std::clamp(display.x * 0.12f, 80.0f, 220.0f);
    float chipHeight = std::clamp(display.y * 0.04f, 28.0f, 40.0f);
    float chipY = 9.0f;
    float chipX1 = 10 + logoSize + 6 + textSize.x + 12.0f;
    float chipGap = std::clamp(display.x * 0.01f, 8.0f, 20.0f);

    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX1, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX1 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(31, 185, 122, 255), chipHeight / 2.0f);
    float iconSize = std::clamp(chipHeight * 0.6f, 14.0f, 24.0f);
    float iconY = chipY + (chipHeight - iconSize) / 2.0f;
    float textY = chipY + (chipHeight - ImGui::GetTextLineHeight()) / 2.0f;
    ImGui::SetCursorPos(ImVec2(chipX1 + 8, iconY));
    ImGui::Image((void*)(intptr_t)chipIcon1.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX1 + 8 + iconSize + 8, textY));
    ImGui::TextColored(ImVec4(1,1,1,1), "Healthy");

    float chipX2 = chipX1 + chipWidth + chipGap;
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX2, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX2 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(255, 165, 0, 220), chipHeight / 2.0f);
    ImGui::SetCursorPos(ImVec2(chipX2 + 8, iconY));
    ImGui::Image((void*)(intptr_t)chipIcon2.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX2 + 8 + iconSize + 8, textY));
    
    ImGui::TextColored(ImVec4(1,1,1,1), "Load: %.2f", avgLoadOfSystem);

    

   

    float windowHeight = ImGui::GetWindowSize().y;
    ImVec2 p1 = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + windowHeight - 1.0f);
    ImVec2 p2 = ImVec2(ImGui::GetWindowPos().x + display.x, ImGui::GetWindowPos().y + windowHeight - 1.0f);
    draw_list->AddLine(p1, p2, IM_COL32(23, 43, 58, 255), 4.0f);
    

    ImGui::PopFont();
    ImGui::End();
    return refreshed;
}