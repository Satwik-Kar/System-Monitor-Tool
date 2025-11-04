#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

int main() {
    
    sf::RenderWindow window(sf::VideoMode(800, 600), "System Monitor Tool");
    window.setFramerateLimit(60);

    
    if (!ImGui::SFML::Init(window)) {
        printf("Failed to initialize ImGui-SFML\n");
        return -1;
    }

    sf::Clock deltaClock;

    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("System Monitor Tool");
        ImGui::Text("ImGui + SFML window is working!");
        ImGui::End();
        window.clear(sf::Color(25, 25, 25)); 
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
