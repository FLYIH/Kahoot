#include "MainMenu.h"

MainMenu::MainMenu() {
    backgroundColor = sf::Color(255, 239, 170);

    if (!font.loadFromFile("./arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    joinButton.setSize(sf::Vector2f(200, 50));
    joinButton.setFillColor(sf::Color::White);
    joinButton.setOutlineColor(sf::Color::Black);
    joinButton.setOutlineThickness(3);
    joinButton.setPosition(150, 275);

    joinText.setFont(font);
    joinText.setString("Join Game");
    joinText.setCharacterSize(20);
    joinText.setFillColor(sf::Color::Black);
    sf::FloatRect joinTextBounds = joinText.getLocalBounds();
    joinText.setPosition(
        joinButton.getPosition().x + (joinButton.getSize().x - joinTextBounds.width) / 2 - joinTextBounds.left,
        joinButton.getPosition().y + (joinButton.getSize().y - joinTextBounds.height) / 2 - joinTextBounds.top
    );

    uploadButton.setSize(sf::Vector2f(200, 50));
    uploadButton.setFillColor(sf::Color::White);
    uploadButton.setOutlineColor(sf::Color::Black);
    uploadButton.setOutlineThickness(3);
    uploadButton.setPosition(450, 275);

    uploadText.setFont(font);
    uploadText.setString("Upload Question");
    uploadText.setCharacterSize(20);
    uploadText.setFillColor(sf::Color::Black);
    sf::FloatRect uploadTextBounds = uploadText.getLocalBounds();
    uploadText.setPosition(
        uploadButton.getPosition().x + (uploadButton.getSize().x - uploadTextBounds.width) / 2 - uploadTextBounds.left,
        uploadButton.getPosition().y + (uploadButton.getSize().y - uploadTextBounds.height) / 2 - uploadTextBounds.top
    );
}

void MainMenu::handleEvents(sf::RenderWindow& window, int& state) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (joinButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    std::cout << "Join Game button clicked!" << std::endl;
                    state = 1;
                }

                if (uploadButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    std::cout << "Upload Question button clicked!" << std::endl;
                    state = 2;
                }
            }
        }
    }
}

void MainMenu::render(sf::RenderWindow& window) {
    window.clear(backgroundColor);
    window.draw(joinButton);
    window.draw(joinText);
    window.draw(uploadButton);
    window.draw(uploadText);
    window.display();
}
