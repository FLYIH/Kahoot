#include "test_client.h"
#include <iostream>
#include <string>
#include "client.h"

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(_WIN32) || defined(__LITTLE_ENDIAN__)
    #define htobe64(x) __builtin_bswap64(x)
    #define be64toh(x) __builtin_bswap64(x)
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
      defined(__BIG_ENDIAN__)
    #define htobe64(x) (x)
    #define be64toh(x) (x)
#else
    #error "Unknown byte order"
#endif

std::string send_file(int sockfd, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        return "Error: Cannot open file.";
    }

    const char *file_name = strrchr(file_path, '/');
    file_name = file_name ? file_name + 1 : file_path;

    char sendline[MAXLINE];
    snprintf(sendline, sizeof(sendline), "UPLOAD %s\n", file_name);
    Writen(sockfd, sendline, strlen(sendline));

    fseek(file, 0, SEEK_END);
    int64_t file_size = ftell(file);
    rewind(file);

    int64_t net_file_size = htobe64(file_size);
    Writen(sockfd, &net_file_size, sizeof(net_file_size));

    char buffer[MAXLINE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        Writen(sockfd, buffer, bytes_read);
    }

    fclose(file);

    if (Readline(sockfd, buffer, MAXLINE) > 0) {
        return std::string(buffer);
    }

    return "No response from server.";
}


void run_test_client(sf::RenderWindow& window, int& state, int sockfd, const char *server_ip, const char *name) {
    char sendline[MAXLINE];
    snprintf(sendline, sizeof(sendline), "%s\n", name);
    Writen(sockfd, sendline, strlen(sendline));

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return;
    }

    // Text fields
    sf::Text promptText("Enter File Path :", font, 20);
    promptText.setFillColor(sf::Color::Black);
    promptText.setPosition(300, 200);

    sf::RectangleShape fileInputBox(sf::Vector2f(300, 40));
    fileInputBox.setPosition(250, 250);
    fileInputBox.setFillColor(sf::Color::White);
    fileInputBox.setOutlineColor(sf::Color::Black);
    fileInputBox.setOutlineThickness(2);

    sf::String fileInput;
    sf::Text fileDisplay("", font, 20);
    fileDisplay.setFillColor(sf::Color::Black);
    fileDisplay.setPosition(260, 255);

    sf::Text responseDisplay("", font, 20);
    responseDisplay.setFillColor(sf::Color::Red);
    responseDisplay.setPosition(250, 300);

    bool showCursor = true;
    sf::Clock cursorClock;

    while (window.isOpen() && state == 6) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                close_connection(sockfd);
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    if (!fileInput.isEmpty()) {
                        fileInput.erase(fileInput.getSize() - 1);
                    }
                } else if (event.text.unicode == '\r') {
                    if (!fileInput.isEmpty()) {
                        std::string filePath = fileInput.toAnsiString();
                        snprintf(sendline, sizeof(sendline), "%s", filePath.c_str());
                        std::string serverResponse = send_file(sockfd, sendline);
                        responseDisplay.setString(serverResponse);
                        fileInput.clear();
                        
                        if (serverResponse == "success\n") {
                            state = 8;
                        }
                    }
                } else if (event.text.unicode < 128) {
                    fileInput += static_cast<char>(event.text.unicode);
                }
            }
        }

        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        fileDisplay.setString(fileInput + (showCursor ? "|" : ""));

        window.clear(sf::Color(255, 239, 170));
        window.draw(promptText);
        window.draw(fileInputBox);
        window.draw(fileDisplay);
        window.draw(responseDisplay);
        window.display();
    }
}
