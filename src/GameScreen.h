#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include <SFML/Graphics.hpp>
#include "client.h"

class GameScreen {
public:
    GameScreen(const char *server_ip, int server_port);
    ~GameScreen();

    void handleEvents(sf::RenderWindow &window);
    void update();
    void render(sf::RenderWindow &window);

private:
    int server_socket;
    sf::RectangleShape questionBox;
    sf::Text questionText;
    sf::Font font;

    std::string currentQuestion;
    std::string currentAnswer;
    void fetchQuestionFromServer();
    void sendAnswerToServer();
};

#endif
