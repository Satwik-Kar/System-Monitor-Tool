#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

void RenderProcessHeaderBar(sf::Texture& listIcon, sf::Texture& searchIcon,
                            sf::Texture& sortIcon, sf::Texture& statusIcon,
                            ImFont* font);
