#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "chip_renderer.hpp"

void RenderMemoryWindow(sf::Texture& memoryTexture,
                     sf::Texture& diskIcon,
                     sf::Texture& swapIcon,
                     sf::Texture& usedIcon,
                     sf::Texture& cachedIcon,
                     sf::Texture& freeIcon,
                     ImFont* font);
