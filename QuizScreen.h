#ifndef QUIZSCREEN_H
#define QUIZSCREEN_H

#include <SFML/Graphics.hpp>
#include <string>

int run_quiz_screen(sf::RenderWindow& window, int& state, int sockfd, bool& correct);

#endif
