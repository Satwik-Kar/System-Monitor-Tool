#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

bool RenderHeaderBar(sf::Texture& logoTexture,
                     sf::Texture& chipIcon1, sf::Texture& chipIcon2,
                     sf::Texture& clockIcon, sf::Texture& refreshIcon,
                     ImFont* font);
