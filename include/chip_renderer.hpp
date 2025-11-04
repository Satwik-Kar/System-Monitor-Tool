#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>

void DrawChip(ImDrawList* drawList, const ImVec2& basePos, float offsetX, float offsetY,
              float width, float height, ImU32 color, sf::Texture& icon,
              const char* label, ImVec4 textColor);
