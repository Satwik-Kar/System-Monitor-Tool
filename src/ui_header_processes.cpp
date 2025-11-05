#include "ui_header_processes.hpp"
#include <algorithm>
#include <iostream>

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

    static char searchBuffer[128] = "";
    static int sortIndex = 0;
    static int statusIndex = 0;
    const char* sortOptions[] = {"CPU %", "Memory %", "PID", "Name"};
    const char* statusOptions[] = {"All", "Running", "Sleeping", "Stopped"};

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