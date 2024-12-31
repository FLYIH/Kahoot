#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "client.h"

struct PlayerRanking {
    std::string name;
    int score;
};

void run_ranking_screen(sf::RenderWindow& window, int& state, int sockfd) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    sf::Color backgroundColor(255, 239, 170);

    sf::Text titleText("Ranking", font, 50);
    titleText.setFillColor(sf::Color(32, 32, 32)); // 深灰色
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(300, 20);

    // 累積分數文字
    int currentScore = 120; // 模擬當前分數
    sf::Text scoreText("Your Score: 120", font, 30);
    scoreText.setFillColor(sf::Color(50, 50, 50)); // 深灰色
    scoreText.setPosition(300, 100);

    // 排名顯示區域
    std::vector<PlayerRanking> rankings = {
        {"Alice", 150},
        {"Bob", 120},
        {"Carol", 100},
        {"Dave", 90},
        {"Eve", 80}
    }; // 模擬伺服器資料

    // 綠色背景的排名框
    sf::RectangleShape rankingBoxes[5];
    sf::Text rankingNumbers[5];
    sf::Text rankingNames[5];
    sf::Text rankingScores[5];

    for (size_t i = 0; i < rankings.size(); ++i) {

        rankingBoxes[i].setSize(sf::Vector2f(700, 50));
        rankingBoxes[i].setFillColor(sf::Color(32, 141, 138));
        rankingBoxes[i].setPosition(50, 200 + i * 65);

        rankingNumbers[i].setFont(font);
        rankingNumbers[i].setCharacterSize(25);
        rankingNumbers[i].setFillColor(sf::Color::White);
        rankingNumbers[i].setString(std::to_string(i + 1));
        rankingNumbers[i].setPosition(
            rankingBoxes[i].getPosition().x + 10, // 左邊對齊
            rankingBoxes[i].getPosition().y + 13
        );

        // 設定名字文字
        rankingNames[i].setFont(font);
        rankingNames[i].setCharacterSize(25);
        rankingNames[i].setFillColor(sf::Color::White);
        rankingNames[i].setString(rankings[i].name);
        sf::FloatRect nameBounds = rankingNames[i].getLocalBounds();
        rankingNames[i].setPosition(
            rankingBoxes[i].getPosition().x + (rankingBoxes[i].getSize().x / 2) - (nameBounds.width / 2),
            rankingBoxes[i].getPosition().y + 13
        );

        // 設定分數文字
        rankingScores[i].setFont(font);
        rankingScores[i].setCharacterSize(25);
        rankingScores[i].setFillColor(sf::Color::White);
        rankingScores[i].setString(std::to_string(rankings[i].score));
        sf::FloatRect scoreBounds = rankingScores[i].getLocalBounds();
        rankingScores[i].setPosition(
            rankingBoxes[i].getPosition().x + rankingBoxes[i].getSize().x - scoreBounds.width - 10, // 右邊對齊
            rankingBoxes[i].getPosition().y + 13
        );
    }

    // // 從伺服器讀取排名資料（模擬資料時註解掉）
    // auto fetchRankingFromServer = [&]() {
    //     char recvline[MAXLINE];
    //     while (Readline(sockfd, recvline, MAXLINE) > 0) {
    //         std::string line(recvline);
    //         std::istringstream iss(line);

    //         PlayerRanking pr;
    //         iss >> pr.name >> pr.score;
    //         rankings.push_back(pr);
    //     }
    // };

    // // 讀取當前分數（模擬資料時註解掉）
    // auto fetchCurrentScoreFromServer = [&]() {
    //     char recvline[MAXLINE];
    //     if (Readline(sockfd, recvline, MAXLINE) > 0) {
    //         currentScore = atoi(recvline);
    //         scoreText.setString("Your Score: " + std::to_string(currentScore));
    //     }
    // };

    // // 初始化讀取
    // fetchCurrentScoreFromServer();
    // fetchRankingFromServer();

    while (window.isOpen() && state == 5) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 清除背景並繪製內容
        window.clear(backgroundColor);
        window.draw(titleText);
        window.draw(scoreText);

        for (size_t i = 0; i < rankings.size(); ++i) {
            window.draw(rankingBoxes[i]);
            window.draw(rankingNumbers[i]);
            window.draw(rankingNames[i]);
            window.draw(rankingScores[i]);
        }

        window.display();
    }
}
