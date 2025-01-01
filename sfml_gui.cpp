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

    // Text fields
    sf::Text promptText("Enter Your Name", font, 20);
    promptText.setFillColor(sf::Color::Black);
    promptText.setPosition(300, 200);

    // Create input box for name
    sf::RectangleShape nameInputBox(sf::Vector2f(300, 40));
    nameInputBox.setPosition(250, 250);
    nameInputBox.setFillColor(sf::Color::White);
    nameInputBox.setOutlineColor(sf::Color::Black);
    nameInputBox.setOutlineThickness(2);

    // Create text display for name input
    sf::String nameInput;
    sf::Text nameDisplay("", font, 20);
    nameDisplay.setFillColor(sf::Color::Black);
    nameDisplay.setPosition(260, 255); // Inside the input box

    bool showCursor = true;
    sf::Clock cursorClock;
    bool enteringName = true;

    while (window.isOpen() && state == 1) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                close_connection(sockfd);
            }

            // Handle text input
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!nameInput.isEmpty()) {
                        nameInput.erase(nameInput.getSize() - 1);
                    }
                } else if (event.text.unicode == '\r') { // Enter key
                    if (!nameInput.isEmpty()) {
                        // Submit the name to the server
                        std::string name = nameInput.toAnsiString();
                        send_name(sockfd, name.c_str());
                        // for test (real : 2)
                        state = 4;
                        return;
                    }
                } else if (event.text.unicode < 128) { // Normal characters
                    nameInput += static_cast<char>(event.text.unicode);
                }
            }
        }

        // Cursor blinking logic
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        // Update displayed text
        nameDisplay.setString(nameInput + (showCursor ? "|" : ""));

        // Draw UI
        window.clear(sf::Color(255, 239, 170)); // Light yellow background
        window.draw(promptText);
        window.draw(nameInputBox);
        window.draw(nameDisplay);
        window.display();
    }
}
