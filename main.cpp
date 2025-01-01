#include <SFML/Graphics.hpp>
#include "MainMenu.h"
#include "sfml_gui.h"
#include "WaitingScreen.h"
#include "QuizScreen.h"
#include "RankingScreen.h"
#include "ResultScreen.h"
#include "client.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Kahoot");
    int state = 0; // 0: 主選單
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
            //std::cout << "waiting\n";
            run_waiting_screen(window, state, sockfd);
            //state = 0;
        } else if (state == 3) {
            run_quiz_screen(window, state, sockfd);
            //state = 0;
        } else if (state == 4) {
            //std::cout << "result\n";
            test_result_screen(window, state, sockfd);
        } else if (state == 5) {
            run_ranking_screen(window, state, sockfd);
        } else if (state == 6) {
            std::cout << "upoad question";
        }
    }

    if (sockfd != -1) {
        close_connection(sockfd);
    }

    return 0;
}
