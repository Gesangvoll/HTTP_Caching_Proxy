#ifndef HANDLE_REQUESTS_HPP
#define HANDLE_REQUESTS_HPP
#include "http_request.hpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unistd.h>

// Network
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Return send status
int send_message_to_server(int to_server_fd, std::vector<char> buff,
                           size_t len);

void connect_server(const char *hostname, const char *port, int id);

void handle(int cl_socket, HttpRequest &request);

#endif
