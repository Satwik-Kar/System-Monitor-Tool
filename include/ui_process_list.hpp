#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>

 
void RenderProcessHeaderBar(sf::Texture& listIcon, sf::Texture& searchIcon,
                            sf::Texture& sortIcon, sf::Texture& statusIcon,
                            ImFont* font);

void RenderProcessList(ImFont* font);