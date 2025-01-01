#ifndef RESULTSCREEN_H
#define RESULTSCREEN_H

#include <SFML/Graphics.hpp>

// 選項結果資料結構
struct OptionResult {
    char optionText[256];
    int correctCount;
    bool isCorrect;
};

// 回合結果資料結構
struct RoundResult {
    OptionResult options[4];
    int playerScore;
    bool playerCorrect;
};

void run_result_screen(sf::RenderWindow& window, int& state, const RoundResult& roundResult, int sockfd);
void test_result_screen(sf::RenderWindow& window, int& state, int sockfd);

#endif
