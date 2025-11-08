#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "ui_header.hpp"
#include "ui_cpu_window.hpp"
#include "ui_memory_window.hpp"
#include "ui_process_list.hpp"

int main() {
    sf::Texture logoTexture, chipIcon1, chipIcon2, refreshIcon, cpuTexture,
                speedIcon, clockIcon, userIcon, systemIcon, idleIcon,
                memoryTexture,searchTexture,sortIcon, filterIcon,menuTexture, diskIcon,swapIcon,usedIcon,cachedIcon,freeIcon;

    if (!logoTexture.loadFromFile("assets/icons/processor.png") ||
        !chipIcon1.loadFromFile("assets/icons/health.png") ||
        !chipIcon2.loadFromFile("assets/icons/pulse.png") ||
        !refreshIcon.loadFromFile("assets/icons/refresh.png") ||
        !cpuTexture.loadFromFile("assets/icons/cpu.png") ||
        !speedIcon.loadFromFile("assets/icons/speed.png") ||
        !clockIcon.loadFromFile("assets/icons/clockspeed.png") ||
        !userIcon.loadFromFile("assets/icons/user.png") ||
        !systemIcon.loadFromFile("assets/icons/system.png") ||
        !idleIcon.loadFromFile("assets/icons/idle.png")||

        !memoryTexture.loadFromFile("assets/icons/memoryTexture.png")||
        !diskIcon.loadFromFile("assets/icons/diskIcon.png")||
        !usedIcon.loadFromFile("assets/icons/usedIcon.png")||
        !swapIcon.loadFromFile("assets/icons/swapIcon.png")||
        !searchTexture.loadFromFile("assets/icons/search.png")||
        !sortIcon.loadFromFile("assets/icons/sort.png")||
        !filterIcon.loadFromFile("assets/icons/filter.png")||
        !cachedIcon.loadFromFile("assets/icons/usedIcon.png")||
        !menuTexture.loadFromFile("assets/icons/menu.png")||
        !freeIcon.loadFromFile("assets/icons/usedIcon.png"
    )) return -1;

    sf::VideoMode vm = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(vm, "System Monitor Tool", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
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
            if (event.type == sf::Event::Resized) {
                sf::View v = window.getView();
                v.setSize((float)event.size.width, (float)event.size.height);
                v.setCenter((float)event.size.width/2.f, (float)event.size.height/2.f);
                window.setView(v);
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color(11, 15, 20)); 

        bool refreshed = RenderHeaderBar(logoTexture, chipIcon1, chipIcon2, clockIcon, refreshIcon, momoFont);
        
        RenderCPUWindow(cpuTexture, speedIcon, clockIcon, userIcon, systemIcon, idleIcon, momoFont);
        RenderMemoryWindow(memoryTexture, diskIcon, swapIcon, usedIcon, cachedIcon, freeIcon, momoFont);
        RenderProcessHeaderBar(menuTexture,searchTexture, sortIcon, filterIcon, momoFont);
        RenderProcessList(momoFont);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}