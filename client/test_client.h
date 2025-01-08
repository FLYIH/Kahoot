#ifndef TEST_CLIENT_H
#define TEST_CLIENT_H

#include <SFML/Graphics.hpp>

std::string send_file(int sockfd, const char *file_path);
void run_test_client(sf::RenderWindow& window, int& state, int sockfd, const char *server_ip, const char *name);

#endif