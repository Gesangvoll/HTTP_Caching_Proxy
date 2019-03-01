#ifndef LOG_HPP
#define LOG_HPP
#include "proxyserver.h"
#include <ctime>
#include <fstream>
#include <mutex>
#include <thread>

char *get_time();

void log_request(HttpRequest &request, std::fstream &fs);


void log_requesting(HttpRequest &request, std::fstream &fs);

void log_responding(HttpResponse &response, std::fstream &fs);

void log_received_response(HttpResponse &response, std::fstream &fs);


#endif
