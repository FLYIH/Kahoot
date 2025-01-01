#include "WaitingScreen.h"
#include "client.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>

void run_waiting_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // "Number of Players" Text
    sf::Text numberText("0", font, 26);
    numberText.setFillColor(sf::Color::White);
    numberText.setPosition(65, 205);

    // Add a green box behind the numberText
    sf::RectangleShape numberBox(sf::Vector2f(50, 50));
    numberBox.setFillColor(sf::Color(32, 141, 138));
    numberBox.setPosition(50, 200);

    // "Kahoot" Text
    sf::Text kahootText("Kahoot", font, 50);
    kahootText.setFillColor(sf::Color(32, 141, 138));
    kahootText.setPosition(330, 180);

    // Game start message text
    sf::Text gameStartText("", font, 30);
    gameStartText.setFillColor(sf::Color::Red);
    gameStartText.setPosition(220, 300); // Position it prominently at the top

    // Name Boxes and Texts (predefined)
    sf::RectangleShape nameBoxes[3];
    sf::Text nameTexts[3];

    for (int i = 0; i < 3; ++i) {
        nameBoxes[i].setSize(sf::Vector2f(400, 50));
        nameBoxes[i].setFillColor(sf::Color::White);
        nameBoxes[i].setOutlineThickness(2);
        nameBoxes[i].setOutlineColor(sf::Color::Black);
        nameBoxes[i].setPosition(200, 300 + i * 60);

        nameTexts[i].setFont(font);
        nameTexts[i].setCharacterSize(20);
        nameTexts[i].setFillColor(sf::Color::Black);
        nameTexts[i].setPosition(210, 310 + i * 60);
    }

    char recvline[MAXLINE]; // Buffer to store received messages
    char lastRecvline[MAXLINE] = ""; // Store the last processed message
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

        // Clear recvline before reading
        memset(recvline, 0, sizeof(recvline));

        // Use non-blocking read to prevent blocking the loop
        if (Readline(sockfd, recvline, MAXLINE) > 0) {
            if (strcmp(recvline, lastRecvline) == 0) {
                continue;
            }
            strncpy(lastRecvline, recvline, sizeof(lastRecvline));

            if (strstr(recvline, "Game starts") != NULL) {
                // Set the game start message text
                gameStartText.setString(recvline);

                // Clear all names and boxes
                //for (int i = 0; i < 3; ++i) {
                    //nameTexts[i].setString(""); // Clear name text
                //}
                //window.draw(gameStartText);
                //continue;
            }

            if (strcmp(recvline, "GameStart\n") == 0) {
                state = 3;
                return;
            }

            if (strcmp(recvline, "waiting\n") != 0 && gameStartText.getString().isEmpty()) {
                char number[10] = {0}, name1[50] = {0}, name2[50] = {0}, name3[50] = {0};

                // Efficient string parsing
                sscanf(recvline, "%9s %49s %49s %49s", number, name1, name2, name3);

                numberText.setString(number);

                // Update names dynamically
                const char* names[3] = {name1, name2, name3};
                for (int i = 0; i < 3; ++i) {
                    nameTexts[i].setString(names[i][0] ? names[i] : "");
                }
            }
        }

        // Draw the screen
        window.clear(sf::Color(255, 239, 170));
        window.draw(numberBox);
        window.draw(numberText);
        window.draw(kahootText);

        // Draw the game start message if it exists
        if (!gameStartText.getString().isEmpty()) {
            window.draw(gameStartText); // Draw "Game starts" message
        }

        // Only draw name boxes and texts if the text is not empty
        for (int i = 0; i < 3; ++i) {
            if (!nameTexts[i].getString().isEmpty()) {
                window.draw(nameBoxes[i]);
                window.draw(nameTexts[i]);
            }
        }

        window.display();
    }
}
