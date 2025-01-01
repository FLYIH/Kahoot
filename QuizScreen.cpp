#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <cstring>
#include "client.h"

char* wrapText(const char* text, sf::Font& font, unsigned int characterSize, float maxWidth) {
    static char wrappedText[1024];
    char currentLine[1024] = "";
    char temp[1024];
    wrappedText[0] = '\0';

    sf::Text sfText;
    sfText.setFont(font);
    sfText.setCharacterSize(characterSize);

    for (const char* c = text; *c; c++) {
        size_t len = strlen(currentLine);
        currentLine[len] = *c;
        currentLine[len + 1] = '\0';

        sfText.setString(currentLine);

        if (sfText.getLocalBounds().width > maxWidth) {
            char* lastSpace = strrchr(currentLine, ' ');
            if (lastSpace) {
                size_t pos = lastSpace - currentLine;
                strncpy(temp, currentLine, pos);
                temp[pos] = '\0';
                strcat(wrappedText, temp);
                strcat(wrappedText, "\n");
                memmove(currentLine, currentLine + pos + 1, strlen(currentLine) - pos);
            } else {
                strcat(wrappedText, currentLine);
                strcat(wrappedText, "\n");
                currentLine[0] = '\0';
            }
        }
    }

    strcat(wrappedText, currentLine);
    return wrappedText;
}

void run_quiz_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // UI elements
    sf::Text questionText("", font, 30);
    questionText.setFillColor(sf::Color::Black);
    questionText.setPosition(100, 50);

    sf::RectangleShape optionButtons[4];
    sf::Text optionTexts[4];
    sf::Color defaultColor(255, 239, 170);
    sf::Color selectedColor(32, 141, 138);
    sf::Color textColor(85, 71, 53);

    for (int i = 0; i < 4; i++) {
        optionButtons[i].setSize(sf::Vector2f(300, 60));
        optionButtons[i].setFillColor(defaultColor);
        optionButtons[i].setPosition(100, 150 + i * 80);

        optionTexts[i].setFont(font);
        optionTexts[i].setCharacterSize(20);
        optionTexts[i].setFillColor(textColor);
    }

    sf::RectangleShape timerBar(sf::Vector2f(600, 20));
    timerBar.setFillColor(sf::Color(255, 204, 0));
    timerBar.setPosition(100, 500);
    sf::Clock timerClock;
    float timeLimit = 10.0f;

    // State variables
    bool answered = false;
    int selectedAnswer = -1;
    char buffer[MAXLINE] = "";

    char recvline[MAXLINE] = "";
    // Receive and process server message
        
    /*while (n = Readline(sockfd, recvline, MAXLINE) > 0) {
        //strncpy(serverMessage, recvline, sizeof(serverMessage) - 1);
        recvline[n] = 0;
        if (strstr(recvline, "Question starts") != NULL) {
            questionText.setString(recvline); // Show countdown
        } else if (strcmp(recvline, "QuestionStart\n") == 0) {
        // Expect question and options in next lines
            n = Readline(sockfd, recvline, MAXLINE);
            recvline[n] = 0;
            questionText.setString(wrapText(recvline, font, 30, 600));

            for (int i = 0; i < 4; i++) {
                n = Readline(sockfd, recvline, MAXLINE);
                recvline[n] = 0;
                optionTexts[i].setString(wrapText(recvline, font, 20, 280));
            }
            break;
        }
    } */

    // Main loop
    while (window.isOpen() && state == 3) {
        sf::Event event;

        // 非阻塞接收伺服器資料
        int n = recv(sockfd, buffer, MAXLINE - 1, MSG_DONTWAIT);
        if (n > 0) {
            buffer[n] = '\0'; // 確保字串結尾

            if (strcmp(buffer, "timeout\n") == 0) {
                state = 4; // 切換狀態
                return;
            }
            if (strstr(buffer, "Question starts") != NULL) {
                questionText.setString(buffer); // 顯示倒數
            } else if (strcmp(buffer, "QuestionStart\n") == 0) {
                // 接收題目
                n = recv(sockfd, buffer, MAXLINE - 1, 0);
                buffer[n] = '\0';
                questionText.setString(wrapText(buffer, font, 30, 600));

                for (int i = 0; i < 4; i++) {
                    n = recv(sockfd, buffer, MAXLINE - 1, 0);
                    buffer[n] = '\0';
                    optionTexts[i].setString(wrapText(buffer, font, 20, 280));
                }
            }
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && !answered) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                for (int i = 0; i < 4; i++) {
                    if (optionButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        for (int j = 0; j < 4; j++) {
                            optionButtons[j].setFillColor(defaultColor);
                        }

                        selectedAnswer = i;
                        optionButtons[i].setFillColor(selectedColor);
                        answered = true;

                        // Send answer to server
                        char answerMessage[1024];
                        snprintf(answerMessage, sizeof(answerMessage), "%s\n", optionTexts[i].getString().toAnsiString().c_str());
                        send(sockfd, answerMessage, strlen(answerMessage), 0);
                        std::cout << "Answer: " << optionTexts[i].getString().toAnsiString() << std::endl;
                    }
                }
            }
        }

        /*if (n = Readline(sockfd, recvline, MAXLINE) > 0) {
            recvline[n] = 0;
            if(strcmp(recvline, "info1\n") == 0) {
                state = 4;
                return;
            }
        } */

        // Timer logic
        float elapsedTime = timerClock.getElapsedTime().asSeconds();
        float timeRemaining = timeLimit - elapsedTime;
        if (timeRemaining > 0) {
            timerBar.setSize(sf::Vector2f(600 * (timeRemaining / timeLimit), 20));
        } else {
            timerBar.setSize(sf::Vector2f(0, 20));
            if (!answered) {
                std::cout << "Time is up!" << std::endl;
                const char* timeoutMessage = "Answer: Timeout\n";
                send(sockfd, timeoutMessage, strlen(timeoutMessage), 0);
                answered = true;
            }
            //state = 4;
        }

        // Draw UI
        window.clear(sf::Color::White);
        window.draw(questionText);
        for (int i = 0; i < 4; i++) {
            window.draw(optionButtons[i]);
            window.draw(optionTexts[i]);
        }
        window.draw(timerBar);
        window.display();
    }
}
