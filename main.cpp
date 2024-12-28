#include <SFML/Graphics.hpp>
#include "MainMenu.h"
#include "sfml_gui.h"
#include "client.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kahoot");
    int state = 0; // 0: 主選單, 1: 遊戲畫面, 2: 上傳畫面
    std::string ipAddress; // Store IP address
    int sockfd = -1;       // Socket file descriptor for server connection

    MainMenu mainMenu;

    while (window.isOpen()) {
        if (state == 0) {
            mainMenu.handleEvents(window, state, ipAddress, sockfd);
            mainMenu.render(window);
        } else if (state == 1) {
            if (sockfd != -1) {
                run_sfml_gui(window, state, sockfd);
            } else {
                std::cerr << "Server connection is not established.\n";
                state = 0;
            }
        } else if (state == 2) {
            std::cout << "Upload screen placeholder\n";
            state = 0;
        }
    }

    if (sockfd != -1) {
        close_connection(sockfd);
    }

    return 0;
}
