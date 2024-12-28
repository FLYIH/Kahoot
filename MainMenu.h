#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

class MainMenu {
public:
    MainMenu();
    void handleEvents(sf::RenderWindow& window, int& state, std::string& ipAddress, int& sockfd);
    void render(sf::RenderWindow& window);

private:
    sf::RectangleShape joinButton;
    sf::RectangleShape uploadButton;
    sf::RectangleShape ipBox; // IP 輸入框背景
    sf::Text joinText;
    sf::Text uploadText;
    sf::Text ipText;
    sf::Text ipDisplay;
    sf::Font font;
    sf::Color backgroundColor;
    sf::String ipInput;
    bool showCursor;
    sf::Clock cursorClock;
};

#endif
