#include <SFML/Graphics.hpp>
#include "MainMenu.h"
#include "GameScreen.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kahoot");

    int state = 0; // 0: 主選單, 1: 遊戲畫面, 2: 上傳題目
    MainMenu mainMenu;
    //GameScreen gameScreen("127.0.0.1", 8080); // 伺服器 IP 與 Port

    while (window.isOpen()) {
        if (state == 0) {
            mainMenu.handleEvents(window, state);
            mainMenu.render(window);
        }
        else if (state == 1) {
            // 遊戲畫面 (後續擴展)
            //gameScreen.handleEvents(window);
            //gameScreen.update();
            //gameScreen.render(window);
        }
        else if (state == 2) {
            // 上傳題目畫面 (後續擴展)
        }
    }

    return 0;
}