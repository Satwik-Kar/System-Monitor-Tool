#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

int main() {
    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("assets/icons/processor.png")) return -1;

    sf::Texture chipIcon1;
    if (!chipIcon1.loadFromFile("assets/icons/health.png")) return -1;

    sf::Texture chipIcon2;
    if (!chipIcon2.loadFromFile("assets/icons/pulse.png")) return -1;

    sf::Texture refreshIcon;
    if (!refreshIcon.loadFromFile("assets/icons/refresh.png")) return -1;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "System Monitor Tool",
        sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
    window.setPosition(sf::Vector2i(0, 0));
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window)) return -1;

    ImGuiIO& io = ImGui::GetIO();
    ImFont* momoFont = io.Fonts->AddFontFromFileTTF("assets/fonts/MomoTrustDisplay-Regular.ttf", 20.0f);
    if (!momoFont) return -1;
    ImGui::SFML::UpdateFontTexture();

    sf::Clock deltaClock;
    bool refreshed = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color(11, 15, 20));

        sf::RectangleShape header(sf::Vector2f(window.getSize().x, 50));
        header.setFillColor(sf::Color(14, 20, 24));
        window.draw(header);

        ImGui::PushFont(momoFont);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 50));
        ImGui::Begin("Header", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings);

        ImGui::SetCursorPos(ImVec2(10, 9));
        ImGui::Image((void*)logoTexture.getNativeHandle(), ImVec2(32, 32));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(47, 15));
        ImGui::Text("System Monitor Tool");
        ImGui::SameLine();

        ImVec2 textSize = ImGui::CalcTextSize("System Monitor Tool");
        float chipWidth = 120.0f, chipHeight = 32.0f, chipY = 9;
        float chipX1 = 47 + textSize.x + 20;
        float chipX2 = chipX1 + chipWidth + 15;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        draw_list->AddRectFilled(
            ImVec2(ImGui::GetWindowPos().x + chipX1, ImGui::GetWindowPos().y + chipY),
            ImVec2(ImGui::GetWindowPos().x + chipX1 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
            IM_COL32(0, 180, 0, 255),
            chipHeight / 2.0f
        );

        float iconSize = 25.0f;
        float iconY = chipY + (chipHeight - iconSize) / 2.0f;
        float textY = chipY + (chipHeight - ImGui::GetTextLineHeight()) / 2.0f;

        ImGui::SetCursorPos(ImVec2(chipX1 + 8, iconY));
        ImGui::Image((void*)chipIcon1.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(chipX1 + 36, textY));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Healthy");

        draw_list->AddRectFilled(
            ImVec2(ImGui::GetWindowPos().x + chipX2, ImGui::GetWindowPos().y + chipY),
            ImVec2(ImGui::GetWindowPos().x + chipX2 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
            IM_COL32(255, 165, 0, 220),
            chipHeight / 2.0f
        );

        ImGui::SetCursorPos(ImVec2(chipX2 + 8, iconY));
        ImGui::Image((void*)chipIcon2.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(chipX2 + 36, textY));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Load 62%%");

        float refreshWidth = 140.0f;
        float refreshX = ImGui::GetIO().DisplaySize.x - refreshWidth - 20;
        float refreshY = 9.0f;
        ImU32 chipColor = IM_COL32(23, 43, 58, 255);
        ImU32 hoverColor = IM_COL32(33, 63, 78, 255);
        float cornerRadius = 10.0f;  

        draw_list->AddRectFilled(
            ImVec2(ImGui::GetWindowPos().x + refreshX, ImGui::GetWindowPos().y + refreshY),
            ImVec2(ImGui::GetWindowPos().x + refreshX + refreshWidth, ImGui::GetWindowPos().y + refreshY + chipHeight),
            chipColor,
            cornerRadius
        );

        ImGui::SetCursorPos(ImVec2(refreshX + 8, iconY));
        ImGui::Image((void*)refreshIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(refreshX + 42, textY));
        ImGui::InvisibleButton("refresh_chip", ImVec2(refreshWidth, chipHeight));

        draw_list->AddText(
            ImVec2(ImGui::GetWindowPos().x + refreshX + 42, ImGui::GetWindowPos().y + chipY + 7),
            IM_COL32(255, 255, 255, 255),
            "Refresh"
        );

        if (ImGui::IsItemHovered()) {
            draw_list->AddRectFilled(
                ImVec2(ImGui::GetWindowPos().x + refreshX, ImGui::GetWindowPos().y + refreshY),
                ImVec2(ImGui::GetWindowPos().x + refreshX + refreshWidth, ImGui::GetWindowPos().y + refreshY + chipHeight),
                hoverColor,
                cornerRadius
            );
            draw_list->AddText(
                ImVec2(ImGui::GetWindowPos().x + refreshX + 42, ImGui::GetWindowPos().y + chipY + 7),
                IM_COL32(255, 255, 255, 255),
                "Refresh"
            );
        }

        if (ImGui::IsItemClicked()) {
            refreshed = true;
            std::cout << "Refreshed!\n";
        }

        ImGui::PopFont();
        ImGui::End();

        ImGui::PushFont(momoFont);
        ImGui::Begin("Main Panel");
        if (refreshed) ImGui::Text("Refreshed successfully!");
        else ImGui::Text("ImGui + SFML window is working!");
        ImGui::End();
        ImGui::PopFont();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
