#include "chip_renderer.hpp"

void DrawChip(ImDrawList* drawList, const ImVec2& basePos, float offsetX, float offsetY,
              float width, float height, ImU32 color, sf::Texture& icon,
              const char* label, ImVec4 textColor) {
    float radius = height / 2.0f;
    ImVec2 chipMin(basePos.x + offsetX, basePos.y + offsetY);
    ImVec2 chipMax(basePos.x + offsetX + width, basePos.y + offsetY + height);
    drawList->AddRectFilled(chipMin, chipMax, color, radius);

    float iconSize = 22.0f;
    float iconY = offsetY + (height - iconSize) * 0.5f;
    float textY = offsetY + (height - ImGui::GetTextLineHeight()) * 0.5f - 1.0f;

    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + 10, basePos.y + iconY));
    ImGui::Image((void*)icon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + 10 + iconSize + 10, basePos.y + textY));
    ImGui::TextColored(textColor, label);
}
