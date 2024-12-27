#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

int main() {
    // 建立視窗
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kahoot Main Menu");

    // 設定背景顏色
    sf::Color backgroundColor = sf::Color(255, 239, 170); // 淡黃色 (篇鵝黃色)

    // 按鈕 - Join Game
    sf::RectangleShape joinButton(sf::Vector2f(200, 50));
    joinButton.setFillColor(sf::Color::White); // 白底
    joinButton.setOutlineColor(sf::Color::Black); // 黑色外框
    joinButton.setOutlineThickness(3);
    joinButton.setPosition(150, 275); // 左側位置

    sf::Font font;
    if (!font.loadFromFile("./arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::Text joinText("Join Game", font, 20);
    joinText.setFillColor(sf::Color::Black);
    sf::FloatRect joinTextBounds = joinText.getLocalBounds();
    joinText.setPosition(
        joinButton.getPosition().x + (joinButton.getSize().x - joinTextBounds.width) / 2 - joinTextBounds.left,
        joinButton.getPosition().y + (joinButton.getSize().y - joinTextBounds.height) / 2 - joinTextBounds.top
    );

    // 按鈕 - Upload Question
    sf::RectangleShape uploadButton(sf::Vector2f(200, 50));
    uploadButton.setFillColor(sf::Color::White); // 白底
    uploadButton.setOutlineColor(sf::Color::Black); // 黑色外框
    uploadButton.setOutlineThickness(3);
    uploadButton.setPosition(450, 275); // 右側位置

    sf::Text uploadText("Upload Question", font, 20);
    uploadText.setFillColor(sf::Color::Black);
    sf::FloatRect uploadTextBounds = uploadText.getLocalBounds();
    uploadText.setPosition(
        uploadButton.getPosition().x + (uploadButton.getSize().x - uploadTextBounds.width) / 2 - uploadTextBounds.left,
        uploadButton.getPosition().y + (uploadButton.getSize().y - uploadTextBounds.height) / 2 - uploadTextBounds.top
    );

    // 主程式循環
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // 處理視窗關閉事件
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // 偵測滑鼠點擊
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // 檢查是否點擊到 Join Game 按鈕
                    if (joinButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        std::cout << "Join Game button clicked!" << std::endl;
                    }

                    // 檢查是否點擊到 Upload Question 按鈕
                    if (uploadButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        std::cout << "Upload Question button clicked!" << std::endl;
                    }
                }
            }
        }

        // 繪製畫面
        window.clear(backgroundColor);
        window.draw(joinButton);
        window.draw(joinText);
        window.draw(uploadButton);
        window.draw(uploadText);
        window.display();
    }

    return 0;
}
