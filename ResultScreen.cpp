#include "ResultScreen.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "client.h"


// 顯示結果的頁面
void run_result_screen(sf::RenderWindow& window, int& state, const RoundResult& roundResult, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // 背景顏色
    sf::Color backgroundColor(255, 239, 170);

    // 標題文字
    sf::Text titleText("Result", font, 50);
    titleText.setFillColor(sf::Color(32, 141, 138));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(300, 20);

    // 玩家是否正確的文字
    std::string correctText = roundResult.playerCorrect ? "You are Correct!" : "You are Wrong!";
    sf::Text correctStatus(correctText, font, 30);
    correctStatus.setFillColor(roundResult.playerCorrect ? sf::Color(32, 141, 138) : sf::Color(200, 0, 0));
    correctStatus.setPosition(100, 100);

    // 玩家得分文字
    sf::Text scoreText("You got " + std::to_string(roundResult.playerScore) + " score.", font, 30);
    scoreText.setFillColor(sf::Color(50, 50, 50));
    scoreText.setPosition(100, 150);

    // 選項結果
    sf::Text optionTexts[4];
    sf::RectangleShape optionBoxes[4];
    for (size_t i = 0; i < 4; ++i) {
        // 設定框
        optionBoxes[i].setSize(sf::Vector2f(500, 50));
        optionBoxes[i].setFillColor(roundResult.options[i].isCorrect ? sf::Color(32, 141, 138) : sf::Color(128, 128, 128, 128));
        optionBoxes[i].setPosition(100, 250 + i * 70);

        // 設定文字
        std::ostringstream oss;
        oss << roundResult.options[i].optionText << " - : " << roundResult.options[i].correctCount;
        optionTexts[i].setFont(font);
        optionTexts[i].setString(oss.str());
        optionTexts[i].setCharacterSize(20);
        optionTexts[i].setFillColor(sf::Color::White);

        sf::FloatRect textBounds = optionTexts[i].getLocalBounds();
        optionTexts[i].setPosition(
            optionBoxes[i].getPosition().x + 10,
            optionBoxes[i].getPosition().y + (50 - textBounds.height) / 2 - textBounds.top
        );
    }

    char recvline[MAXLINE];
    // 繪製迴圈
    while (window.isOpen() && state == 4) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // 可在此處添加按鍵或按鈕事件來結束頁面
            if (Readline(sockfd, recvline, MAXLINE) > 0) {
                if(strcmp(recvline, "info2\n") == 0) {
                    state = 5;
                }
            }
        }

        // 繪製畫面
        window.clear(backgroundColor);
        window.draw(titleText);
        window.draw(correctStatus);
        window.draw(scoreText);

        for (size_t i = 0; i < 4; ++i) {
            window.draw(optionBoxes[i]);
            window.draw(optionTexts[i]);
        }

        window.display();
    }
}

void test_result_screen(sf::RenderWindow& window, int& state,int sockfd) {
    RoundResult roundResult = {
        {
            {"Paris", 15, true},
            {"London", 5, false},
            {"Berlin", 2, false},
            {"Rome", 1, false}
        },
        10,
        false
    };
    
    run_result_screen(window, state, roundResult, sockfd);
}