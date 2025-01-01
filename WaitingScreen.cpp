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

    // Name Texts
    sf::Text name1Text("", font, 26);
    name1Text.setFillColor(sf::Color::Black);
    name1Text.setPosition(150, 200);

    sf::Text name2Text("", font, 26);
    name2Text.setFillColor(sf::Color::Black);
    name2Text.setPosition(150, 250);

    // Add a green box behind the numberText
    sf::RectangleShape numberBox(sf::Vector2f(50, 50)); // Size of the box
    numberBox.setFillColor(sf::Color(32, 141, 138)); // 綠色背景
    numberBox.setPosition(50, 200); // Align the box to the left corner

    char recvline[MAXLINE]; // Buffer to store received messages
    bool read = false;
    int n;
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

        if (n = Readline(sockfd, recvline, MAXLINE) > 0) {
            recvline[n] = 0;
            if (strstr(recvline, "Game starts") != NULL) {
                continue;
            } 

            if (strcmp(recvline, "GameStart\n") == 0) {
                state = 3;
                return;
            }
            
            if (strcmp(recvline, "waiting\n") != 0) {
                char number[10] = {0}, name1[50] = {0}, name2[50] = {0};

                // Parse the received line
                char* token = strtok(recvline, " ");
                if (token != NULL) {
                    strncpy(number, token, sizeof(number) - 1);
                    token = strtok(NULL, " ");
                }
                if (token != NULL) {
                    strncpy(name1, token, sizeof(name1) - 1);
                    token = strtok(NULL, " ");
                }
                if (token != NULL) {
                    strncpy(name2, token, sizeof(name2) - 1);
                }
                numberText.setString(number);
                name1Text.setString(name1);
                name2Text.setString(name2);
            }
        }

        // Draw the screen
        window.clear(sf::Color(255, 239, 170)); // Light yellow background
        window.draw(numberBox);   // Draw the green box
        window.draw(numberText);  // Draw the number text
        window.draw(name1Text);   // Draw the first name
        window.draw(name2Text);   // Draw the second name
        window.display();
    }
}
