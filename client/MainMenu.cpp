#include "MainMenu.h"
#include "client.h"

MainMenu::MainMenu() {
    backgroundColor = sf::Color(255, 239, 170);

    if (!font.loadFromFile("./arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // IP 輸入框標籤
    ipText.setFont(font);
    ipText.setString("Enter IP Address");
    ipText.setCharacterSize(20);
    ipText.setFillColor(sf::Color::Black);
    ipText.setPosition(320, 50);

    // IP 輸入框矩形
    ipBox.setSize(sf::Vector2f(400, 40));
    ipBox.setFillColor(sf::Color::White);
    ipBox.setOutlineColor(sf::Color::Black);
    ipBox.setOutlineThickness(2);
    ipBox.setPosition(220, 90);

    // IP 輸入框內容顯示
    ipDisplay.setFont(font);
    ipDisplay.setCharacterSize(20);
    ipDisplay.setFillColor(sf::Color::Black);
    ipDisplay.setPosition(230, 95);

    // Join Game 按鈕
    joinButton.setSize(sf::Vector2f(200, 50));
    joinButton.setFillColor(sf::Color::White);
    joinButton.setOutlineColor(sf::Color::Black);
    joinButton.setOutlineThickness(3);
    joinButton.setPosition(150, 275);

    joinText.setFont(font);
    joinText.setString("Join Game");
    joinText.setCharacterSize(20);
    joinText.setFillColor(sf::Color::Black);
    sf::FloatRect joinTextBounds = joinText.getLocalBounds();
    joinText.setPosition(
        joinButton.getPosition().x + (joinButton.getSize().x - joinTextBounds.width) / 2 - joinTextBounds.left,
        joinButton.getPosition().y + (joinButton.getSize().y - joinTextBounds.height) / 2 - joinTextBounds.top
    );

    uploadButton.setSize(sf::Vector2f(200, 50));
    uploadButton.setFillColor(sf::Color::White);
    uploadButton.setOutlineColor(sf::Color::Black);
    uploadButton.setOutlineThickness(3);
    uploadButton.setPosition(450, 275);

    uploadText.setFont(font);
    uploadText.setString("Upload Question");
    uploadText.setCharacterSize(20);
    uploadText.setFillColor(sf::Color::Black);
    sf::FloatRect uploadTextBounds = uploadText.getLocalBounds();
    uploadText.setPosition(
        uploadButton.getPosition().x + (uploadButton.getSize().x - uploadTextBounds.width) / 2 - uploadTextBounds.left,
        uploadButton.getPosition().y + (uploadButton.getSize().y - uploadTextBounds.height) / 2 - uploadTextBounds.top
    );

    ipInput = "";
    showCursor = true;
}

void MainMenu::handleEvents(sf::RenderWindow& window, int& state, std::string& ipAddress, int& sockfd) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b') { // Backspace
                if (!ipInput.isEmpty()) {
                    ipInput.erase(ipInput.getSize() - 1);
                }
            } else if (event.text.unicode < 128) { // Normal characters
                ipInput += static_cast<char>(event.text.unicode);
            }
            
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (joinButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    ipAddress = ipInput;
                    sockfd = connect_to_server(ipAddress.c_str());
                    if (sockfd < 0) {
                        std::cerr << "Failed to connect to server\n";
                        return;
                    }
                    state = 1; // 切換到遊戲畫面
                }

                if (uploadButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    ipAddress = ipInput;
                    sockfd = connect_to_server(ipAddress.c_str());
                    if (sockfd < 0) {
                        std::cerr << "Failed to connect to server\n";
                        return;
                    }
                    state = 6; // 切換到上傳畫面
                }
            }
        }
    }

    if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
        showCursor = !showCursor;
        cursorClock.restart();
    }
}

void MainMenu::render(sf::RenderWindow& window) {
    window.clear(backgroundColor);

    ipDisplay.setString(ipInput + (showCursor ? "|" : ""));

    window.draw(ipText);
    window.draw(ipBox); 
    window.draw(ipDisplay);
    window.draw(joinButton);
    window.draw(joinText);
    window.draw(uploadButton);
    window.draw(uploadText);

    window.display();
}
