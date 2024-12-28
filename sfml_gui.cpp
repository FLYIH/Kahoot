#include "sfml_gui.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "client.h"

void run_sfml_gui(sf::RenderWindow& window, int& state) {
    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // Text fields and buttons
    sf::Text ipText("Enter IP Address:", font, 20);
    sf::Text messageText("Enter Message:", font, 20);
    sf::Text sendButtonText("Submit", font, 20);
    sf::Text serverResponseText("Server Response:", font, 20);
    sf::Text backButtonText("Back to Menu", font, 20);

    // 設置文字顏色為黑色
    ipText.setFillColor(sf::Color::Black);
    messageText.setFillColor(sf::Color::Black);
    sendButtonText.setFillColor(sf::Color::Black);
    serverResponseText.setFillColor(sf::Color::Black);
    backButtonText.setFillColor(sf::Color::Black);

    ipText.setPosition(50, 50);
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

    sf::String ipInput, messageInput;
    sf::Text ipDisplay("", font, 20);
    sf::Text messageDisplay("", font, 20);
    sf::Text serverDisplay("", font, 20);

    // 設置輸入與回應文字顏色為黑色
    ipDisplay.setFillColor(sf::Color::Black);
    messageDisplay.setFillColor(sf::Color::Black);
    serverDisplay.setFillColor(sf::Color::Black);

    ipDisplay.setPosition(210, 50);
    messageDisplay.setPosition(210, 150);
    serverDisplay.setPosition(210, 350);

    bool isEnteringIP = true;
    bool showCursor = true;
    sf::Clock cursorClock;

    int sockfd = -1;

    while (window.isOpen() && state == 1) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                if (sockfd != -1) {
                    close_connection(sockfd);
                }
            }

            // Handle button clicks
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (sendButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    // 按下 Submit 按鈕
                    if (sockfd == -1) {
                        std::string ip = ipInput.toAnsiString();
                        sockfd = connect_to_server(ip.c_str());
                        if (sockfd < 0) {
                            std::cerr << "Failed to connect to server\n";
                            window.close();
                        }
                    }

                    // 發送訊息
                    if (!isEnteringIP) {
                        std::string msg = messageInput.toAnsiString();
                        std::string serverResponse;
                        char responseBuffer[1024];

                        try {
                            send_and_receive(sockfd, msg.c_str(), responseBuffer, sizeof(responseBuffer));
                            serverResponse = responseBuffer;
                        } catch (const std::runtime_error &e) {
                            std::cerr << e.what() << std::endl;
                            window.close();
                        }

                        // 顯示伺服器回應
                        messageInput.clear();
                        serverDisplay.setString(serverResponse);
                    }
                } else if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    // 返回主選單
                    state = 0;
                }
            }

            // Handle text input
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (isEnteringIP && !ipInput.isEmpty()) {
                        ipInput.erase(ipInput.getSize() - 1);
                    } else if (!isEnteringIP && !messageInput.isEmpty()) {
                        messageInput.erase(messageInput.getSize() - 1);
                    }
                } else if (event.text.unicode == '\r') { // Enter key
                    if (isEnteringIP) {
                        isEnteringIP = false; // 切換到訊息輸入階段
                    }
                } else if (event.text.unicode < 128) { // Normal characters
                    if (isEnteringIP) {
                        ipInput += static_cast<char>(event.text.unicode);
                    } else {
                        messageInput += static_cast<char>(event.text.unicode);
                    }
                }
            }
        }

        // Cursor blinking
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        // Update display text with cursor
        ipDisplay.setString(isEnteringIP ? ipInput + (showCursor ? "|" : "") : ipInput);
        messageDisplay.setString(!isEnteringIP ? messageInput + (showCursor ? "|" : "") : messageInput);

        // Draw UI
        window.clear(sf::Color(255, 239, 213)); // 設置背景為鵝黃色
        window.draw(ipText);
        window.draw(messageText);
        window.draw(sendButton);
        window.draw(sendButtonText);
        window.draw(serverResponseText);
        window.draw(backButton);
        window.draw(backButtonText);
        window.draw(ipDisplay);
        window.draw(messageDisplay);
        window.draw(serverDisplay);
        window.display();
    }
}
