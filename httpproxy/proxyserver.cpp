#include "proxyserver.h"
#include "log.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>

#define MAX_REQUEST 500
#define MAXSIZE 60000

using namespace std;
fstream fs("/var/log/erss/proxy.log", fstream::app);
pthread_mutex_t mylock;

void send_message_to_server(int to_server_fd, vector<char> buff, size_t len) {
  string dehuh(buff.begin(), buff.end());
  string newbuff = dehuh;
  /* int pos = newbuff.find("\r\n");
  string firstline = newbuff.substr(0, pos+2);
  int pos2 = newbuff.find("Host:");
  int pos3 = newbuff.find("\r\n", pos2);
  newbuff.erase(0, pos2);
  string hostname = newbuff.substr(0,pos3+2-pos2);
  string connectclose = "Connection: close\r\n\r\n";
      int pos4 = newbuff.find("\r\n\r\n");
   newbuff.erase(0,pos4+4);
  string sendinfohhh = firstline + hostname + connectclose + newbuff;
  */

  int pos = newbuff.find("Connection: Keep-Alive");
  string before = newbuff.substr(0, pos);
  newbuff.erase(0, pos + 20);
  string middle = "Connection: close";
  string sendinfohhh = before + middle + newbuff;

  //    cout << firstline << "1\n";
  // cout << hostname << "2\n";
  // cout << connectclose << "3\n";
  // cout << sendinfohhh << "\n";

  int status = send(to_server_fd, sendinfohhh.c_str(), sendinfohhh.size(), 0);
  if (status < 0) {
    cerr << "Error: Send Message to Server Failed!" << endl;
    throw(runtime_error("error"));
  }
}

int connect_server(const char *hostname, const char *port, int id) {
  struct addrinfo hints;
  struct addrinfo *server_addrinfo;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  int status = getaddrinfo(hostname, port, &hints, &server_addrinfo);
  if (status != 0) {
    cerr << "Error: Connect Server getaddrinfo Failed!" << endl;
    throw(runtime_error("error"));
  }
  int to_server_fd =
      socket(server_addrinfo->ai_family, server_addrinfo->ai_socktype,
             server_addrinfo->ai_protocol);
  if (to_server_fd < 0) {
    cerr << "Error: Create to_server_fd Failed!" << endl;
    throw(runtime_error("error"));
  }

  status = connect(to_server_fd, server_addrinfo->ai_addr,
                   server_addrinfo->ai_addrlen);

  if (status == -1) {
    cerr << "Error: cannot connect socket" << endl;
    throw(runtime_error("error"));
  }
  free(server_addrinfo);
  return to_server_fd;
}

void receive_data_from_server(int to_server_fd, HttpResponse &response) {
  response.data_buff.resize(MAXSIZE);
  //    cout << "Start Receiving Data from Server...." << endl;
  pthread_mutex_lock(&mylock);
  response.data_buff_len = 0;
  size_t recv_buff_len = 1;

  while (recv_buff_len != 0) {
    if (recv_buff_len == 1) {
      recv_buff_len =
          recv(to_server_fd, &response.data_buff.data()[0], MAXSIZE, 0);
      response.data_buff_len += recv_buff_len;
    } else {
      recv_buff_len =
          recv(to_server_fd, &response.data_buff.data()[response.data_buff_len],
               MAXSIZE, 0);
      response.data_buff_len += recv_buff_len;
    }
  }
  pthread_mutex_unlock(&mylock);
  string to_parse_data(response.data_buff.begin(), response.data_buff.end());
  //    cout << to_parse_data << "\n";
  response.parse(to_parse_data);
  log_received_response(response, fs);
}

void handleGET(HttpRequest &request, HttpResponse &response, int request_id) {
  request.id = request_id;
  int to_server_fd = connect_server(request.server_name.c_str(),
                                    request.server_port.c_str(), request_id);
  send_message_to_server(to_server_fd, request.data, request.data.size());
  receive_data_from_server(to_server_fd, response);
  close(to_server_fd);
}

void handlePOST(HttpRequest &request, HttpResponse &response, int request_id) {
  request.id = request_id;
  int to_server_fd = connect_server(request.server_name.c_str(),
                                    request.server_port.c_str(), request_id);
  send_message_to_server(to_server_fd, request.data, request.data.size());
  receive_data_from_server(to_server_fd, response);
  close(to_server_fd);
}

void handleCONNECT(HttpRequest &request, HttpResponse &response,
                   int cl_socket) {

  string empty_str;
  int to_server_fd = connect_server(request.server_name.c_str(),
                                    request.server_port.c_str(), request.id);

  string response200ok = "HTTP/1.1 200 Connection Established\r\n\r\n";

  send(cl_socket, response200ok.c_str(), strlen(response200ok.c_str()), 0);

  fd_set readfds;
  while (true) {
    int maxfd = 0;
    FD_ZERO(&readfds);
    FD_SET(cl_socket, &readfds);
    FD_SET(to_server_fd, &readfds);
    if (cl_socket > to_server_fd) {
      maxfd = cl_socket;
    } else {
      maxfd = to_server_fd;
    }

    int status = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if (status == -1) {
      std::cerr << "Error: select failed\n";
    }
    if (FD_ISSET(cl_socket, &readfds)) {
      char buffer[MAXSIZE];
      memset(&buffer, 0, MAXSIZE);
      size_t recv_len = recv(cl_socket, &buffer, MAXSIZE, 0);

      if (recv_len == 0) {
        close(to_server_fd);
        break;
      }

      if (recv_len < 0) {
        break;
      }

      status = send(to_server_fd, &buffer, recv_len, 0);
      if (status < 0) {
        break;
      }

    } else if (FD_ISSET(to_server_fd, &readfds)) {
      char recvbuffer[MAXSIZE];
      memset(&recvbuffer, 0, MAXSIZE);
      // cout << "Start Receiving Data from Server...." << endl;

      size_t recv_len = recv(to_server_fd, &recvbuffer, MAXSIZE, 0);
      if (recv_len < 0) {
        break;
      }
      if (recv_len == 0) {
        close(to_server_fd);
        break;
      }
      status = send(cl_socket, &recvbuffer, recv_len, 0);
      if (status < 0) {
        break;
      }
    }
  }
  close(to_server_fd);
  return;
}

void respond_to_client(int cl_socket, vector<char> data_buff,
                       size_t data_buff_len) {
  string final(data_buff.begin(), data_buff.end());

  int status = send(cl_socket, data_buff.data(), data_buff_len, 0);
  if (status < 0) {
    cerr << "Error: Can not respond to client!" << endl;
  }
}

void try_proxy(client_info our_info) {
  HttpRequest request;
  ssize_t status1;
  status1 = recv(our_info.cl_socket, &request.data.data()[0], MAXSIZE, 0);
  string clininfo(request.data.begin(), request.data.end());
  // cout << clininfo << "\n";
  if (status1 == -1) {
    cerr << "Error: recv from client wrong!\n" << endl;
  }
  request.request_setting();
  // cout << "Hostname" << request.method << "\n";
  // cout << request.server_name << "\n" << request.server_port << "\n";
  request.id = our_info.request_id;
  request.ip = our_info.ip;
  HttpResponse response;
  response.id = our_info.request_id;
  if (request.method == "GET") {
    handleGET(request, response, our_info.request_id);

    log_request(request, fs);
    log_responding(response, fs);
    respond_to_client(our_info.cl_socket, response.data_buff,
                      response.data_buff.size());
  } else if (request.method == "POST") {
    handlePOST(request, response, our_info.request_id);
    log_request(request, fs);
    log_responding(response, fs);
    respond_to_client(our_info.cl_socket, response.data_buff,
                      response.data_buff.size());
  } else if (request.method == "CONNECT") {
    handleCONNECT(request, response, our_info.cl_socket);
    log_request(request, fs);
  } else {
    cerr << "Error: The Proxy Can Not Handle This Method!\n" << endl;
  }
  close(our_info.cl_socket);
}

int main(int argc, char *argv[]) {

  signal(SIGPIPE, SIG_IGN);
  int socket_fd;
  struct addrinfo proxy_info;
  struct addrinfo *proxy_info_list;
  const char *proxy_hostname = NULL;
  ssize_t status;
  if (argc != 2) {
    cerr << "Error: input format for proxyserver must be: <port_num>\n "
         << endl;
    return EXIT_FAILURE;
  }

  char *proxy_port = argv[1];

  string ourargv(argv[1]);
  if (ourargv.find_first_not_of("0123456789") != string::npos) {
    cerr << "Error: Parameters must be numbers\n" << endl;
  }

  memset(&proxy_info, 0, sizeof(proxy_info));
  proxy_info.ai_family = AF_UNSPEC;
  proxy_info.ai_socktype = SOCK_STREAM;
  proxy_info.ai_flags = AI_PASSIVE;

  status =
      getaddrinfo(proxy_hostname, proxy_port, &proxy_info, &proxy_info_list);

  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << proxy_hostname << "," << proxy_port << ")" << endl;
    return -1;
  }

  socket_fd = socket(proxy_info_list->ai_family, proxy_info_list->ai_socktype,
                     proxy_info_list->ai_protocol);

  if (socket_fd == -1) {
    cerr << "Error: cannnot create socket" << endl;
    cerr << "  (" << proxy_hostname << "," << proxy_port << ")" << endl;
    return -1;
  }

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  status =
      bind(socket_fd, proxy_info_list->ai_addr, proxy_info_list->ai_addrlen);

  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << proxy_hostname << "," << proxy_port << ")" << endl;
    return -1;
  }

  status = listen(socket_fd, MAX_REQUEST);

  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl;
    cerr << "  (" << proxy_hostname << "," << proxy_port << ")" << endl;
    return -1;
  }

  int thread_id = 0;
  cout << "running" << endl;
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd =
        accept(socket_fd, (struct sockaddr *)&client_addr, &addr_len);
    char ipad[20];
    strcpy(ipad, inet_ntoa(client_addr.sin_addr));
    string ip_address(ipad);

    if (client_fd == -1) {
      cerr << "Error: can't accept client connection" << endl;
      return -1;
    }

    client_info cl_client(client_fd, thread_id++,
                          inet_ntoa(client_addr.sin_addr));

    cl_client.ip = ip_address;
    if (thread_id < MAXSIZE) {
      thread task(try_proxy, cl_client);
      task.detach();
    }
  }
  free(proxy_info_list);
  fs.close();
}
