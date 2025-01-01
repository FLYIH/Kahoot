#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"

#define MAX_PLAYERS 4

struct PlayerRanking {
    char name[50];
    int score;
};

int compareScores(const void* a, const void* b) {
    PlayerRanking* playerA = (PlayerRanking*)a;
    PlayerRanking* playerB = (PlayerRanking*)b;
    return playerB->score - playerA->score;
}

void run_final_screen(sf::RenderWindow& window, int& state, int sockfd) {
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

    int currentScore = 0;
    sf::Text scoreText("Your Score: 120", font, 30);
    scoreText.setFillColor(sf::Color(50, 50, 50)); // 深灰色
    scoreText.setPosition(300, 100);

    PlayerRanking rankings[MAX_PLAYERS];

    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    bool infoReceived = false;


    // 綠色背景的排名框
    sf::RectangleShape rankingBoxes[MAX_PLAYERS];
    sf::Text rankingNumbers[MAX_PLAYERS];
    sf::Text rankingNames[MAX_PLAYERS];
    sf::Text rankingScores[MAX_PLAYERS];

    for (int i = 0; i < MAX_PLAYERS; ++i) {
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

    while (window.isOpen() && state == 5) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // 使用 select 檢查是否有資料可讀
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 10000; // 10 毫秒

        int retval = select(sockfd + 1, &readfds, NULL, NULL, &tv);

        if (retval > 0 && FD_ISSET(sockfd, &readfds)) {
            char recvline[MAXLINE];
            if (Readline(sockfd, recvline, MAXLINE) > 0) {
                // std::cout << "read score\n";
                if (strstr(recvline, "GameOver\n") != NULL) {
                    // close window
                    window.close();
                    state = 0; // 設定 state 為 0，表示遊戲結束
                    return; // 離開 function
                }
                if (strcmp(recvline, "FinalInfo\n") == 0) {
                    char names[MAX_PLAYERS][50];
                    int scores[MAX_PLAYERS];
                    Readline(sockfd, recvline, MAXLINE);
                    // 解析接收到的資料
                    int a, b ,c, d;
                    sscanf(recvline, "%s %s %s %s %d %d %d %d %d %d %d %d",
                        names[0], names[1], names[2], names[3], &a, &b, &c, &d,
                        &scores[0], &scores[1], &scores[2], &scores[3]);

                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        strncpy(rankings[i].name, names[i], sizeof(rankings[i].name) - 1);
                        rankings[i].name[sizeof(rankings[i].name) - 1] = '\0';
                        rankings[i].score = scores[i];

                        // 更新圖形介面的名字和分數
                        rankingNames[i].setString(rankings[i].name);
                        sf::FloatRect nameBounds = rankingNames[i].getLocalBounds();
                        rankingNames[i].setPosition(
                            rankingBoxes[i].getPosition().x + (rankingBoxes[i].getSize().x / 2) - (nameBounds.width / 2),
                            rankingBoxes[i].getPosition().y + 13
                        );

                        rankingScores[i].setString(std::to_string(rankings[i].score));
                        sf::FloatRect scoreBounds = rankingScores[i].getLocalBounds();
                        rankingScores[i].setPosition(
                            rankingBoxes[i].getPosition().x + rankingBoxes[i].getSize().x - scoreBounds.width - 10,
                            rankingBoxes[i].getPosition().y + 13
                        );
                    }

                    // 排序
                    qsort(rankings, MAX_PLAYERS, sizeof(PlayerRanking), compareScores);
                }
            }
        }

        // 繪製畫面
        window.clear(backgroundColor);
        window.draw(titleText);
        window.draw(scoreText);

        for (int i = 0; i < MAX_PLAYERS; ++i) {
            window.draw(rankingBoxes[i]);
            window.draw(rankingNumbers[i]);
            window.draw(rankingNames[i]);
            window.draw(rankingScores[i]);
        }

        window.display();
    }

}
