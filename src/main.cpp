#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "ui_header.hpp"
#include "ui_cpu_window.hpp"

int main() {
    sf::Texture logoTexture, chipIcon1, chipIcon2, refreshIcon, cpuTexture,
                speedIcon, clockIcon, userIcon, systemIcon, idleIcon;

    if (!logoTexture.loadFromFile("assets/icons/processor.png") ||
        !chipIcon1.loadFromFile("assets/icons/health.png") ||
        !chipIcon2.loadFromFile("assets/icons/pulse.png") ||
        !refreshIcon.loadFromFile("assets/icons/refresh.png") ||
        !cpuTexture.loadFromFile("assets/icons/cpu.png") ||
        !speedIcon.loadFromFile("assets/icons/speed.png") ||
        !clockIcon.loadFromFile("assets/icons/clockspeed.png") ||
        !userIcon.loadFromFile("assets/icons/user.png") ||
        !systemIcon.loadFromFile("assets/icons/system.png") ||
        !idleIcon.loadFromFile("assets/icons/idle.png")) return -1;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "System Monitor Tool",
        sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) return -1;

    ImGuiIO& io = ImGui::GetIO();
    ImFont* momoFont = io.Fonts->AddFontFromFileTTF("assets/fonts/MomoTrustDisplay-Regular.ttf", 20.0f);
    if (!momoFont) return -1;
    ImGui::SFML::UpdateFontTexture();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color(11, 15, 20));

        bool refreshed = RenderHeaderBar(logoTexture, chipIcon1, chipIcon2, clockIcon, refreshIcon, momoFont);
        RenderCPUWindow(cpuTexture, speedIcon, clockIcon, userIcon, systemIcon, idleIcon, momoFont);

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
