#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>

void DrawChip(ImDrawList* drawList, const ImVec2& basePos, float offsetX, float offsetY,
              float width, float height, ImU32 color, sf::Texture& icon,
              const char* label, ImVec4 textColor) {
    float radius = height / 2.0f;
    ImVec2 chipMin(basePos.x + offsetX, basePos.y + offsetY);
    ImVec2 chipMax(basePos.x + offsetX + width, basePos.y + offsetY + height);
    drawList->AddRectFilled(chipMin, chipMax, color, radius);

    float iconSize = 22.0f;
    float iconY = offsetY + (height - iconSize) * 0.5f;
    float textY = offsetY + (height - ImGui::GetTextLineHeight()) * 0.5f - 1.0f;

    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + 10, basePos.y + iconY));
    ImGui::Image((void*)icon.getNativeHandle(), ImVec2(iconSize, iconSize));
    ImGui::SameLine();
    ImGui::SetCursorScreenPos(ImVec2(chipMin.x + 10 + iconSize + 10, basePos.y + textY));
    ImGui::TextColored(textColor, label);
}

int main() {
    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("assets/icons/processor.png")) return -1;
    sf::Texture chipIcon1;
    if (!chipIcon1.loadFromFile("assets/icons/health.png")) return -1;
    sf::Texture chipIcon2;
    if (!chipIcon2.loadFromFile("assets/icons/pulse.png")) return -1;
    sf::Texture refreshIcon;
    if (!refreshIcon.loadFromFile("assets/icons/refresh.png")) return -1;
    sf::Texture cpuTexture;
    if (!cpuTexture.loadFromFile("assets/icons/cpu.png")) return -1;
    sf::Texture speedIcon;
    if (!speedIcon.loadFromFile("assets/icons/speed.png")) return -1;
    sf::Texture clockIcon;
    if (!clockIcon.loadFromFile("assets/icons/clockspeed.png")) return -1;
    sf::Texture userIcon;
    if (!userIcon.loadFromFile("assets/icons/user.png")) return -1;
    sf::Texture systemIcon;
    if (!systemIcon.loadFromFile("assets/icons/system.png")) return -1;
    sf::Texture idleIcon;
    if (!idleIcon.loadFromFile("assets/icons/idle.png")) return -1;
    
    
    
    
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "System Monitor Tool", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
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

        ImGui::GetStyle().WindowRounding = 15.0f;
        ImGui::PushFont(momoFont);

        // HEADER BAR
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 50));
        ImGui::Begin("Header", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground |
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

        // Health Chip
        draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX1, ImGui::GetWindowPos().y + chipY),
            ImVec2(ImGui::GetWindowPos().x + chipX1 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
            IM_COL32(31, 185, 122, 255), chipHeight / 2.0f);
        float iconSize = 20.0f;
        float iconY = chipY + (chipHeight - iconSize) / 2.0f;
        float textY = chipY + (chipHeight - ImGui::GetTextLineHeight()) / 2.0f;
        ImGui::SetCursorPos(ImVec2(chipX1 + 8, iconY));
        ImGui::Image((void*)chipIcon1.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(chipX1 + 36, textY));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Healthy");

        // Load Chip
        draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX2, ImGui::GetWindowPos().y + chipY),
            ImVec2(ImGui::GetWindowPos().x + chipX2 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
            IM_COL32(255, 165, 0, 220), chipHeight / 2.0f);
        ImGui::SetCursorPos(ImVec2(chipX2 + 8, iconY));
        ImGui::Image((void*)chipIcon2.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(chipX2 + 36, textY));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "Load 62%%");

        // GHz Chip
        float chipX3 = chipX2 + chipWidth + 15;
        draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + chipX3, ImGui::GetWindowPos().y + chipY),
            ImVec2(ImGui::GetWindowPos().x + chipX3 + chipWidth, ImGui::GetWindowPos().y + chipY + chipHeight),
            IM_COL32(70, 130, 180, 255), chipHeight / 2.0f);
        ImGui::SetCursorPos(ImVec2(chipX3 + 8, iconY));
        ImGui::Image((void*)clockIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(chipX3 + 36, textY));
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "3.6 GHz");

        // Refresh Chip
        float refreshWidth = 140.0f;
        float refreshX = ImGui::GetIO().DisplaySize.x - refreshWidth - 20;
        float refreshY = 9.0f;
        ImU32 chipColor = IM_COL32(38, 166, 154, 255);
        float cornerRadius = 10.0f;

        draw_list->AddRectFilled(ImVec2(ImGui::GetWindowPos().x + refreshX, ImGui::GetWindowPos().y + refreshY),
            ImVec2(ImGui::GetWindowPos().x + refreshX + refreshWidth, ImGui::GetWindowPos().y + refreshY + chipHeight),
            chipColor, cornerRadius);
        ImGui::SetCursorPos(ImVec2(refreshX + 8, iconY));
        ImGui::Image((void*)refreshIcon.getNativeHandle(), ImVec2(iconSize, iconSize));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(refreshX + 42, textY));
        ImGui::InvisibleButton("refresh_chip", ImVec2(refreshWidth, chipHeight));
        draw_list->AddText(ImVec2(ImGui::GetWindowPos().x + refreshX + 42, ImGui::GetWindowPos().y + chipY + 7),
            IM_COL32(255, 255, 255, 255), "Refresh");

        if (ImGui::IsItemClicked()) {
            refreshed = true;
            std::cout << "Refreshed!\n";
        }

        ImGui::PopFont();
        ImGui::End();

        // CPU USAGE WINDOW
        ImGui::PushFont(momoFont);
        ImGui::SetNextWindowPos(ImVec2(20, 60), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 350));
        ImGui::Begin("CPU Usage", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);

        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::Image((void*)cpuTexture.getNativeHandle(), ImVec2(28, 28));
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
        ImGui::TextColored(ImVec4(255, 255, 255, 255), "CPU Usage");

        // User & System dots
        float rightPadding = 25.0f;
        float dotSize = 9.0f;
        float spacing = 60.0f;
        ImVec2 winPos = ImGui::GetWindowPos();
        float startX = ImGui::GetWindowContentRegionMax().x - rightPadding - 150;
        float baseY = winPos.y + 18.0f;

        ImDrawList* drawListDots = ImGui::GetWindowDrawList();
        drawListDots->AddCircleFilled(ImVec2(winPos.x + startX, baseY), dotSize / 2, IM_COL32(0, 200, 200, 255));
        ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + 12, baseY - 8));
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "User");

        drawListDots->AddCircleFilled(ImVec2(winPos.x + startX + spacing, baseY), dotSize / 2, IM_COL32(120, 180, 255, 255));
        ImGui::SetCursorScreenPos(ImVec2(winPos.x + startX + spacing + 12, baseY - 8));
        ImGui::TextColored(ImVec4(0.7, 0.9, 1, 1), "System");

        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        // AVG + GHZ chips
        ImDrawList* chipDrawList = ImGui::GetWindowDrawList();
        ImVec2 chipPos = ImGui::GetCursorScreenPos();
        DrawChip(chipDrawList, chipPos, 10, 10, 150, 36, IM_COL32(23, 43, 58, 255), speedIcon, "Avg 58%%", ImVec4(0.3, 0.6, 0.8, 1));
        DrawChip(chipDrawList, chipPos, 180, 10, 150, 36, IM_COL32(28, 55, 70, 255), clockIcon, "3.6 GHz", ImVec4(0.4, 0.8, 1, 1));
        ImGui::Dummy(ImVec2(0, 16));

        // ================= LINE GRAPH =================
        static float userCPU[100] = {};
        static float systemCPU[100] = {};
        static int dataIndex = 0;
        static float elapsed = 0.0f;
        const float UPDATE_INTERVAL = 0.2f;

        elapsed += ImGui::GetIO().DeltaTime;
        if (elapsed >= UPDATE_INTERVAL) {
            elapsed = 0.0f;
            float newUser = 30.0f + static_cast<float>(rand() % 70);
            float newSystem = 20.0f + static_cast<float>(rand() % 50);
            userCPU[dataIndex] = newUser;
            systemCPU[dataIndex] = newSystem;
            dataIndex = (dataIndex + 1) % IM_ARRAYSIZE(userCPU);
        }

        ImVec2 graphSize = ImVec2(ImGui::GetContentRegionAvail().x, 120);
        ImVec2 graphPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawListGraph = ImGui::GetWindowDrawList();
        drawListGraph->AddRectFilled(graphPos,
            ImVec2(graphPos.x + graphSize.x, graphPos.y + graphSize.y),
            IM_COL32(15, 25, 35, 255), 8.0f);

        float minVal = 0.0f;
        float maxVal = 100.0f;
        float range = maxVal - minVal;

        for (int i = 1; i < IM_ARRAYSIZE(userCPU); i++) {
            int index0 = (dataIndex + i - 1) % IM_ARRAYSIZE(userCPU);
            int index1 = (dataIndex + i) % IM_ARRAYSIZE(userCPU);
            float x0 = graphPos.x + ((i - 1) / (float)(IM_ARRAYSIZE(userCPU) - 1)) * graphSize.x;
            float x1 = graphPos.x + (i / (float)(IM_ARRAYSIZE(userCPU) - 1)) * graphSize.x;
            float y0_user = graphPos.y + graphSize.y * (1.0f - (userCPU[index0] - minVal) / range);
            float y1_user = graphPos.y + graphSize.y * (1.0f - (userCPU[index1] - minVal) / range);
            float y0_sys = graphPos.y + graphSize.y * (1.0f - (systemCPU[index0] - minVal) / range);
            float y1_sys = graphPos.y + graphSize.y * (1.0f - (systemCPU[index1] - minVal) / range);
            drawListGraph->AddLine(ImVec2(x0, y0_user), ImVec2(x1, y1_user), IM_COL32(0, 200, 200, 255), 2.0f);
            drawListGraph->AddLine(ImVec2(x0, y0_sys), ImVec2(x1, y1_sys), IM_COL32(120, 180, 255, 255), 2.0f);
        }

        ImGui::Dummy(graphSize);

        // === FOUR CHIPS BELOW GRAPH ===
        ImGui::Dummy(ImVec2(0, 20)); 
        ImDrawList* chipDrawList2 = ImGui::GetWindowDrawList();
        ImVec2 chipBase2 = ImGui::GetCursorScreenPos();

        float baseX = 10;
        float xOffset = baseX;
        float height = 56;
        float paddingX = 40.0f; 
        float spacing2 = 20.0f;

       
        struct ChipInfo {
            sf::Texture* icon;
            const char* label;
            ImU32 color;
            ImVec4 textColor;
        };
        std::vector<ChipInfo> chipInfos = {
            { &userIcon, "User: 12%%", IM_COL32(23, 43, 58, 255), ImVec4(0.6, 0.9, 1, 1) },
            { &systemIcon, "System: 30%%", IM_COL32(28, 55, 70, 255), ImVec4(0.4, 0.8, 1, 1) },
            { &idleIcon, "Idle: 48%%", IM_COL32(40, 70, 90, 255), ImVec4(0.5, 0.8, 1, 1) },
            { &cpuTexture, "Available Cores: 16", IM_COL32(50, 90, 110, 255), ImVec4(0.7, 1, 0.9, 1) }
        };

       
        for (const auto& chip : chipInfos) {
            float textWidth = ImGui::CalcTextSize(chip.label).x;
            float totalWidth = textWidth + paddingX + 40.0f; 
            DrawChip(chipDrawList2, chipBase2, xOffset, 0, totalWidth, height, chip.color, *chip.icon, chip.label, chip.textColor);
            xOffset += totalWidth + spacing2; 
        }

        ImGui::Dummy(ImVec2(0, 10));


        ImGui::End();
        ImGui::PopFont();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
