#include "ui_header.hpp"
#include <iostream>

bool RenderHeaderBar(sf::Texture& logoTexture,
                     sf::Texture& chipIcon1, sf::Texture& chipIcon2,
                     sf::Texture& clockIcon, sf::Texture& refreshIcon,
                     ImFont* font) {
    bool refreshed = false;

    ImGui::PushFont(font);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 50));
    ImGui::Begin("Header", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings);

    ImGui::SetCursorPos(ImVec2(10, 9));
    ImGui::Image((void*)logoTexture.getNativeHandle(), ImVec2(32, 32));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(47, 15));
    ImGui::Text("System Monitor Tool");
    ImGui::SameLine();

    ImVec2 textSize = ImGui::CalcTextSize("System Monitor Tool");
    float chipWidth = 120.0f, chipHeight = 32.0f, chipY = 9;
    float chipX1 = 47 + textSize.x + 20;
    float chipX2 = chipX1 + chipWidth + 15;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Health chip
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX1, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX1 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(31, 185, 122, 255), chipHeight / 2.0f);
    float iconSize = 20.0f;
    float iconY = chipY + (chipHeight - iconSize) / 2.0f;
    float textY = chipY + (chipHeight - ImGui::GetTextLineHeight()) / 2.0f;
    ImGui::SetCursorPos(ImVec2(chipX1 + 8, iconY));
    ImGui::Image((void*)chipIcon1.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX1 + 36, textY));
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Healthy");

    // Load chip
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX2, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX2 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(255, 165, 0, 220), chipHeight / 2.0f);
    ImGui::SetCursorPos(ImVec2(chipX2 + 8, iconY));
    ImGui::Image((void*)chipIcon2.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX2 + 36, textY));
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Load 62%%");

    // GHz chip
    float chipX3 = chipX2 + chipWidth + 15;
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX3, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + chipX3 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        IM_COL32(70, 130, 180, 255), chipHeight / 2.0f);
    ImGui::SetCursorPos(ImVec2(chipX3 + 8, iconY));
    ImGui::Image((void*)clockIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(chipX3 + 36, textY));
    ImGui::TextColored(ImVec4(1, 1, 1, 1), "3.6 GHz");

    // Refresh chip
    float refreshWidth = 140.0f;
    float refreshX = ImGui::GetIO().DisplaySize.x - refreshWidth - 20;
    ImU32 chipColor = IM_COL32(38, 166, 154, 255);
    draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + refreshX, ImGui::GetWindowPos().y + chipY),
        ImVec2(ImGui::GetWindowPos().x + refreshX + refreshWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
        chipColor, 10.0f);
    ImGui::SetCursorPos(ImVec2(refreshX + 8, iconY));
    ImGui::Image((void*)refreshIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(refreshX + 42, textY));
    ImGui::InvisibleButton("refresh_chip", ImVec2(refreshWidth, chipHeight));
    draw_list->AddText(ImVec2(ImGui::GetWindowPos().x + refreshX + 42, ImGui::GetWindowPos().y + chipY + 7),
        IM_COL32(255, 255, 255, 255), "Refresh");

    if (ImGui::IsItemClicked()) {
        refreshed = true;
        std::cout << "Refreshed!\n";
    }

    ImGui::PopFont();
    ImGui::End();
    return refreshed;
}
