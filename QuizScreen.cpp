#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "client.h"

// 自動換行函數
std::string wrapText(const std::string& text, sf::Font& font, unsigned int characterSize, float maxWidth) {
    std::string wrappedText;
    std::string currentLine;
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setCharacterSize(characterSize);

    for (char c : text) {
        currentLine += c;
        sfText.setString(currentLine);

        if (sfText.getLocalBounds().width > maxWidth) {
            size_t lastSpace = currentLine.find_last_of(' ');
            if (lastSpace != std::string::npos) {
                wrappedText += currentLine.substr(0, lastSpace) + "\n";
                currentLine = currentLine.substr(lastSpace + 1);
            } else {
                wrappedText += currentLine + "\n";
                currentLine.clear();
            }
        }
    }

    wrappedText += currentLine;
    return wrappedText;
}

void run_quiz_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // 問題文字
    sf::Text questionText("What is the capital of France?", font, 30);
    questionText.setFillColor(sf::Color::Black);
    questionText.setPosition(100, 50);

    // 選項按鈕
    sf::RectangleShape optionButtons[4];
    sf::Text optionTexts[4];
    std::string options[4] = {
        "Paris",
        "London is a very long name city",
        "Berlin with a slightly longer name",
        "Rome"
    };

    // 顏色設定
    sf::Color defaultColor(255, 239, 170);  // 淺黃色
    sf::Color selectedColor(32, 141, 138); // 綠藍色
    sf::Color textColor(85, 71, 53);       // 深棕色

    // 初始化按鈕和文字
    for (int i = 0; i < 4; i++) {
        optionButtons[i].setSize(sf::Vector2f(300, 60));
        optionButtons[i].setFillColor(defaultColor);
        optionButtons[i].setPosition(100, 150 + i * 80);

        // 自動換行處理
        std::string wrappedOption = wrapText(options[i], font, 20, 280);
        optionTexts[i].setFont(font);
        optionTexts[i].setString(wrappedOption);
        optionTexts[i].setCharacterSize(20);
        optionTexts[i].setFillColor(textColor);

        sf::FloatRect textBounds = optionTexts[i].getLocalBounds();
        optionTexts[i].setPosition(
            optionButtons[i].getPosition().x + 10,
            optionButtons[i].getPosition().y + (60 - textBounds.height) / 2 - textBounds.top
        );
    }

    // 計時條
    sf::RectangleShape timerBar(sf::Vector2f(600, 20));
    timerBar.setFillColor(sf::Color(255, 204, 0)); // 金色
    timerBar.setPosition(100, 500);
    sf::Clock timerClock;
    float timeLimit = 10.0f;

    // 狀態處理
    bool answered = false;
    int selectedAnswer = -1; // -1 表示尚未選擇

    // 主迴圈
    while (window.isOpen() && state == 3) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && !answered) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // 檢查按鈕是否被點擊
                for (int i = 0; i < 4; i++) {
                    if (optionButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        // 重設按鈕顏色
                        for (int j = 0; j < 4; j++) {
                            optionButtons[j].setFillColor(defaultColor);
                        }

                        // 設定選擇按鈕的顏色
                        selectedAnswer = i;
                        optionButtons[i].setFillColor(selectedColor);
                        answered = true;

                        // 傳送答案到伺服器
                        std::string answerMessage = "Answer: " + options[i] + "\n";
                        send(sockfd, answerMessage.c_str(), answerMessage.size(), 0);
                        std::cout << "Answer: " << options[i] << std::endl;
                    }
                }
            }
        }

        // 計時邏輯
        float elapsedTime = timerClock.getElapsedTime().asSeconds();
        float timeRemaining = timeLimit - elapsedTime;
        if (timeRemaining > 0) {
            timerBar.setSize(sf::Vector2f(600 * (timeRemaining / timeLimit), 20));
        } else if (!answered) {
            timerBar.setSize(sf::Vector2f(0, 20)); // 時間結束
            std::cout << "Time is up!" << std::endl;
            std::string timeoutMessage = "Answer: Timeout\n";
            send(sockfd, timeoutMessage.c_str(), timeoutMessage.size(), 0);
            answered = true;
        }

        // 繪製介面
        window.clear(sf::Color::White); // 白色背景
        window.draw(questionText);
        for (int i = 0; i < 4; i++) {
            window.draw(optionButtons[i]);
            window.draw(optionTexts[i]);
        }
        window.draw(timerBar);
        window.display();
    }
}
