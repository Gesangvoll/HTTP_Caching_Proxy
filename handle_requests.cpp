#include "handle_requests.hpp"

using namespace std;

int send_message_to_server(int to_server_fd, vector<char> buff, size_t len) {

  return 1;
}

void connect_server(const char *hostname, const char *port, int id) {}

void handle(int cl_socket, HttpRequest &request) {
  ssize_t status2;

  status2 = recv(cl_socket, &request.data, sizeof(request.data), 0);

  request.request_setting();
}
