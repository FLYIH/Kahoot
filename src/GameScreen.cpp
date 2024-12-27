#include "GameScreen.h"
#include <iostream>
#include <unistd.h>

GameScreen::GameScreen(const char *server_ip, int server_port) {
    server_socket = connect_to_server(server_ip, server_port);

    if (!font.loadFromFile("./arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    questionBox.setSize(sf::Vector2f(600, 100));
    questionBox.setFillColor(sf::Color::White);
    questionBox.setOutlineColor(sf::Color::Black);
    questionBox.setOutlineThickness(3);
    questionBox.setPosition(100, 200);

    questionText.setFont(font);
    questionText.setCharacterSize(20);
    questionText.setFillColor(sf::Color::Black);

    fetchQuestionFromServer();
}

GameScreen::~GameScreen() {
    close(server_socket);
}

void GameScreen::handleEvents(sf::RenderWindow &window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\n') { // 按下 Enter 提交答案
                sendAnswerToServer();
                fetchQuestionFromServer();
            } else {
                currentAnswer += static_cast<char>(event.text.unicode);
            }
        }
    }
}

void GameScreen::update() {
    questionText.setString(currentQuestion);
    sf::FloatRect textBounds = questionText.getLocalBounds();
    questionText.setPosition(
        questionBox.getPosition().x + (questionBox.getSize().x - textBounds.width) / 2 - textBounds.left,
        questionBox.getPosition().y + (questionBox.getSize().y - textBounds.height) / 2 - textBounds.top
    );
}

void GameScreen::render(sf::RenderWindow &window) {
    window.clear(sf::Color(255, 239, 170)); // 背景色
    window.draw(questionBox);
    window.draw(questionText);
    window.display();
}

void GameScreen::fetchQuestionFromServer() {
    char buffer[1024];
    receive_from_server(server_socket, buffer, sizeof(buffer));
    currentQuestion = buffer;
}

void GameScreen::sendAnswerToServer() {
    send_to_server(server_socket, currentAnswer.c_str());
    currentAnswer.clear();
}