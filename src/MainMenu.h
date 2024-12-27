#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

class MainMenu {
public:
    MainMenu();
    void handleEvents(sf::RenderWindow& window, int& state);
    void render(sf::RenderWindow& window);
private:
    sf::RectangleShape joinButton;
    sf::RectangleShape uploadButton;
    sf::Text joinText;
    sf::Text uploadText;
    sf::Font font;
    sf::Color backgroundColor;
};

#endif