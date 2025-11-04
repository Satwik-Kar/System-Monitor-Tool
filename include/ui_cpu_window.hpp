#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "chip_renderer.hpp"

void RenderCPUWindow(sf::Texture& cpuTexture,
                     sf::Texture& speedIcon,
                     sf::Texture& clockIcon,
                     sf::Texture& userIcon,
                     sf::Texture& systemIcon,
                     sf::Texture& idleIcon,
                     ImFont* font);
