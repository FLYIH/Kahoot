#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "client.h"

void run_quiz_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // Question Text
    sf::Text questionText("What is the capital of France?", font, 30);
    questionText.setFillColor(sf::Color::Black);
    questionText.setPosition(100, 50);

    // Option Buttons
    sf::RectangleShape optionButtons[4];
    sf::Text optionTexts[4];
    std::string options[4] = {"Paris", "London", "Berlin", "Rome"};
    sf::Color buttonColors[4] = {
        sf::Color(255, 0, 0),   // Red
        sf::Color(0, 0, 255),   // Blue
        sf::Color(0, 255, 0),   // Green
        sf::Color(255, 255, 0)  // Yellow
    };

    for (int i = 0; i < 4; i++) {
        optionButtons[i].setSize(sf::Vector2f(300, 60));
        optionButtons[i].setFillColor(buttonColors[i]);
        optionButtons[i].setPosition(100, 150 + i * 80);

        optionTexts[i].setFont(font);
        optionTexts[i].setString(options[i]);
        optionTexts[i].setCharacterSize(20);
        optionTexts[i].setFillColor(sf::Color::Black);
        optionTexts[i].setPosition(
            optionButtons[i].getPosition().x + 20,
            optionButtons[i].getPosition().y + 15
        );
    }

    // Timer Bar
    sf::RectangleShape timerBar(sf::Vector2f(600, 20));
    timerBar.setFillColor(sf::Color(0, 255, 0)); // Green
    timerBar.setPosition(100, 500);
    sf::Clock timerClock;
    float timeLimit = 10.0f; // 10 seconds for each question

    // Back Button
    sf::RectangleShape backButton(sf::Vector2f(150, 40));
    backButton.setPosition(50, 550);
    backButton.setFillColor(sf::Color::White);
    sf::Text backButtonText("Back to Menu", font, 20);
    backButtonText.setFillColor(sf::Color::Black);
    backButtonText.setPosition(
        backButton.getPosition().x + 15,
        backButton.getPosition().y + 5
    );

    // Answer Handling
    bool answered = false;
    int selectedAnswer = -1; // -1 means no answer selected

    while (window.isOpen() && state == 3) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Check for option button clicks
                for (int i = 0; i < 4; i++) {
                    if (optionButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectedAnswer = i;
                        answered = true;
                        std::cout << "Answer: " << options[i] << std::endl;

                        // Send answer to server (replace with actual logic)
                        std::string answerMessage = "Answer: " + options[i] + "\n";
                        send(sockfd, answerMessage.c_str(), answerMessage.size(), 0);
                    }
                }

                // Check for back button click
                if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    state = 0; // Return to main menu
                    return;
                }
            }
        }

        // Timer logic
        float elapsedTime = timerClock.getElapsedTime().asSeconds();
        float timeRemaining = timeLimit - elapsedTime;
        if (timeRemaining > 0) {
            timerBar.setSize(sf::Vector2f(600 * (timeRemaining / timeLimit), 20));
        } else {
            timerBar.setSize(sf::Vector2f(0, 20)); // Time is up
            if (!answered) {
                std::cout << "Time is up!" << std::endl;
                // Notify server about timeout (replace with actual logic)
                std::string timeoutMessage = "Answer: Timeout\n";
                send(sockfd, timeoutMessage.c_str(), timeoutMessage.size(), 0);
                answered = true;
            }
        }

        // Draw UI
        window.clear(sf::Color(255, 239, 170)); // Background color
        window.draw(questionText);
        for (int i = 0; i < 4; i++) {
            window.draw(optionButtons[i]);
            window.draw(optionTexts[i]);
        }
        window.draw(timerBar);
        window.draw(backButton);
        window.draw(backButtonText);
        window.display();
    }
}
