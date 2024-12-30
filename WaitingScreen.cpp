#include "WaitingScreen.h"
#include "client.h"
#include <SFML/Graphics.hpp>
#include <iostream>

void run_waiting_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // "Number of Players" Text
    sf::Text numberText("0", font, 26);
    numberText.setFillColor(sf::Color::White); // 字體顏色設為白色
    numberText.setPosition(65, 205); // 對齊框內位置

    // Add a green box behind the numberText
    sf::RectangleShape numberBox(sf::Vector2f(50, 50)); // Size of the box
    numberBox.setFillColor(sf::Color(32, 141, 138)); // 綠色背景
    numberBox.setPosition(50, 200); // Align the box to the left corner

    char recvline[MAXLINE]; // Buffer to store received messages
    bool read = false;

    while (window.isOpen() && state == 2) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Simulate receiving a number (replace with real server logic)
        if (!read) {
            readID(sockfd);
            read = true;
        }

        if (Readline(sockfd, recvline, MAXLINE) > 0) {
            if (strcmp(recvline, "start\n") == 0) {
                state = 3;
                return;
            }
            if (strcmp(recvline, "waiting\n") != 0) {
                numberText.setString(recvline);
            }
        }

        // Draw the screen
        window.clear(sf::Color(255, 239, 170)); // Light yellow background
        window.draw(numberBox);   // Draw the green box
        window.draw(numberText);  // Draw the number text
        window.display();
    }
}
