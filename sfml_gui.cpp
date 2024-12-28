#include "sfml_gui.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "client.h"

void run_sfml_gui(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // Text fields and buttons
    sf::Text messageText("Enter Message:", font, 20);
    sf::Text sendButtonText("Submit", font, 20);
    sf::Text serverResponseText("Server Response:", font, 20);
    sf::Text backButtonText("Back to Menu", font, 20);

    // 設置文字顏色為黑色
    messageText.setFillColor(sf::Color::Black);
    sendButtonText.setFillColor(sf::Color::Black);
    serverResponseText.setFillColor(sf::Color::Black);
    backButtonText.setFillColor(sf::Color::Black);

    messageText.setPosition(50, 150);
    sendButtonText.setPosition(260, 300);
    serverResponseText.setPosition(50, 350);
    backButtonText.setPosition(50, 450);

    sf::RectangleShape sendButton(sf::Vector2f(100, 40));
    sendButton.setPosition(250, 300);
    sendButton.setFillColor(sf::Color::White);       // 白底
    sendButton.setOutlineColor(sf::Color::Black);    // 黑框
    sendButton.setOutlineThickness(2);               // 邊框寬度

    sf::RectangleShape backButton(sf::Vector2f(150, 40));
    backButton.setPosition(50, 450);
    backButton.setFillColor(sf::Color::White);
    backButton.setOutlineColor(sf::Color::Black);
    backButton.setOutlineThickness(2);

    sf::String messageInput;
    sf::Text messageDisplay("", font, 20);
    sf::Text serverDisplay("", font, 20);

    // 設置輸入與回應文字顏色為黑色
    messageDisplay.setFillColor(sf::Color::Black);
    serverDisplay.setFillColor(sf::Color::Black);

    messageDisplay.setPosition(210, 150);
    serverDisplay.setPosition(210, 350);

    bool showCursor = true;
    sf::Clock cursorClock;

    while (window.isOpen() && state == 1) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                close_connection(sockfd);
            }

            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (sendButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    // 發送訊息
                    std::string msg = messageInput.toAnsiString();
                    std::string serverResponse;
                    char responseBuffer[1024];

                    try {
                        send_and_receive(sockfd, msg.c_str(), responseBuffer, sizeof(responseBuffer));
                        serverResponse = responseBuffer;
                    } catch (const std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                        window.close();
                    }

                    // 顯示伺服器回應
                    messageInput.clear();
                    serverDisplay.setString(serverResponse);
                } else if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    // 返回主選單
                    state = 0;
                    return;
                }
            }

            // Handle text input
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!messageInput.isEmpty()) {
                        messageInput.erase(messageInput.getSize() - 1);
                    }
                } else if (event.text.unicode < 128) { // Normal characters
                    messageInput += static_cast<char>(event.text.unicode);
                }
            }
        }

        // Cursor blinking
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        // Update display text with cursor
        messageDisplay.setString(messageInput + (showCursor ? "|" : ""));

        // Draw UI
        window.clear(sf::Color(255, 239, 213)); // 設置背景為鵝黃色
        window.draw(messageText);
        window.draw(sendButton);
        window.draw(sendButtonText);
        window.draw(serverResponseText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.draw(messageDisplay);
        window.draw(serverDisplay);
        window.display();
    }
}
