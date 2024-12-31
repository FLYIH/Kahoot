#ifndef RESULTSCREEN_H
#define RESULTSCREEN_H

#include <SFML/Graphics.hpp>

// 選項結果資料結構
struct OptionResult {
    std::string optionText;
    int correctCount;
    bool isCorrect;
};

// 回合結果資料結構
struct RoundResult {
    std::vector<OptionResult> options;
    int playerScore;
    bool playerCorrect;
};

void run_result_screen(sf::RenderWindow& window, int& state, const RoundResult& roundResult, int sockfd);
void test_result_screen(sf::RenderWindow& window, int& state, int sockfd);

#endif
