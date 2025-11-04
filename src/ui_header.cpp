#include "ui_header.hpp"
#include <algorithm>
#include <iostream>

bool RenderHeaderBar(sf::Texture& logoTexture,
                     sf::Texture& chipIcon1, sf::Texture& chipIcon2,
                     sf::Texture& clockIcon, sf::Texture& refreshIcon,
                     ImFont* font) {
    bool refreshed = false;
    ImGui::PushFont(font);
    ImVec2 display = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(display.x, std::clamp(display.y * 0.06f, 40.0f, 80.0f)));
    ImGui::Begin("Header", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings);

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
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

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
    ImGui::TextColored(ImVec4(1,1,1,1), "Load 62%%");

    float chipX3 = chipX2 + chipWidth + chipGap;
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX3, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX3 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(70, 130, 180, 255), chipHeight / 2.0f);
    ImGui::SetCursorPos(ImVec2(chipX3 + 8, iconY));
    ImGui::Image((void*)(intptr_t)clockIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX3 + 8 + iconSize + 8, textY));
    ImGui::TextColored(ImVec4(1,1,1,1), "3.6 GHz");

    float refreshWidth = std::clamp(display.x * 0.14f, 120.0f, 260.0f);
    float refreshX = std::max(chipX3 + chipWidth + chipGap, display.x - refreshWidth - 20.0f);
    ImU32 chipColor = IM_COL32(38, 166, 154, 255);
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + refreshX, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + refreshX + refreshWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        chipColor, chipHeight * 0.25f);
    ImGui::SetCursorPos(ImVec2(refreshX + 8, iconY));
    ImGui::Image((void*)(intptr_t)refreshIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(refreshX + 8 + iconSize + 8, textY));
    ImGui::InvisibleButton("refresh_chip", ImVec2(refreshWidth, chipHeight));
    draw_list->AddText(ImVec2(ImGui::GetWindowPos().x + refreshX + 8 + iconSize + 8, ImGui::GetWindowPos().y + chipY + (chipHeight - ImGui::GetTextLineHeight()) * 0.5f),
        IM_COL32(255, 255, 255, 255), "Refresh");

    if (ImGui::IsItemClicked()) {
        refreshed = true;
        std::cout << "Refreshed!\n";
    }

    ImGui::PopFont();
    ImGui::End();
    return refreshed;
}
