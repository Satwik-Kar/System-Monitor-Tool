#include "chip_renderer.hpp"
#include <algorithm>

void DrawChip(ImDrawList* drawList, const ImVec2& basePos, float offsetX, float offsetY,
              float width, float height, ImU32 color, sf::Texture& icon,
              const char* label, ImVec4 textColor) {
    float radius = std::max(2.0f, height * 0.5f);
    ImVec2 chipMin(basePos.x + offsetX, basePos.y + offsetY);
    ImVec2 chipMax(basePos.x + offsetX + width, basePos.y + offsetY + height);
    drawList->AddRectFilled(chipMin, chipMax, color, radius);

    float iconSize = std::max(14.0f, height * 0.5f);
    float iconY = offsetY + (height - iconSize) * 0.5f;
    float textY = offsetY + (height - ImGui::GetTextLineHeight()) * 0.5f - 1.0f;

    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + std::max(6.0f, width * 0.03f), basePos.y + iconY));
    ImGui::Image((void*)(intptr_t)icon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + std::max(6.0f, width * 0.03f) + iconSize + std::max(6.0f, width * 0.02f), basePos.y + textY));
    ImGui::TextColored(textColor, "%s", label);
}
