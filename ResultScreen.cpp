#include "ResultScreen.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "client.h"


// 顯示結果的頁面
void run_result_screen(sf::RenderWindow& window, int& state, int sockfd, int correctAnswer, bool isCorrect) {
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
    sf::Text correctStatus("", font, 30);
    correctStatus.setFillColor(isCorrect ? sf::Color(32, 141, 138) : sf::Color(200, 0, 0));
    correctStatus.setString(isCorrect ? "You are Correct!" : "You are Wrong!");
    correctStatus.setPosition(100, 100);

    // 玩家得分文字
    int playerScore = isCorrect ? 10 : 0;  // 假設答對得 10 分
    sf::Text scoreText("You got " + std::to_string(playerScore) + " score.", font, 30);
    scoreText.setFillColor(sf::Color(50, 50, 50));
    scoreText.setPosition(100, 150);

    // 選項結果
    sf::Text optionTexts[4];
    sf::RectangleShape optionBoxes[4];

    for (size_t i = 0; i < 4; ++i) {
        optionBoxes[i].setSize(sf::Vector2f(500, 50));
        optionBoxes[i].setFillColor(i == correctAnswer ? sf::Color(32, 141, 138) : sf::Color(128, 128, 128, 128));
        optionBoxes[i].setPosition(100, 250 + i * 70);

        optionTexts[i].setFont(font);
        optionTexts[i].setCharacterSize(20);
        optionTexts[i].setFillColor(sf::Color::White);
    }

    char recvline[MAXLINE];
    fd_set readfds;
    struct timeval tv;

    // 初始化選項結果
    int correctCounts[4] = {0, 0, 0, 0};

    // 繪製迴圈
    while (window.isOpen() && state == 4) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 使用 select 等待數據
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 1000; // 1 毫秒

        int retval = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (retval > 0 && FD_ISSET(sockfd, &readfds)) {
            if (Readline(sockfd, recvline, MAXLINE) > 0) {
                if (strcmp(recvline, "Info\n") == 0) {
                    state = 5;
                    return;
                }
                if (strcmp(recvline, "Info1\n") == 0) {
                    //memset(recvline, 0, sizeof(recvline));
                    Readline(sockfd, recvline, MAXLINE);

                    std::cout << "got score = " << recvline << std::endl;

                    // 解析數據
                    int a, b, c, d;
                    sscanf(recvline, "%d %d %d %d %d %d %d %d", &correctCounts[0], &correctCounts[1], &correctCounts[2], &correctCounts[3], &a, &b, &c, &d);

                    for (size_t i = 0; i < 4; ++i) {
                        std::ostringstream oss;
                        oss << (i + 1) << " - Correct: " << correctCounts[i];
                        optionTexts[i].setString(oss.str());

                        sf::FloatRect textBounds = optionTexts[i].getLocalBounds();
                        optionTexts[i].setPosition(
                            optionBoxes[i].getPosition().x + 10,
                            optionBoxes[i].getPosition().y + (50 - textBounds.height) / 2 - textBounds.top
                        );
                    }
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
