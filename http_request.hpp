#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP
#include <iostream>
#include <string>
#include <vector>

#define MAXSIZE 65507

class HttpRequest {
public:
  std::vector<char> data;
  std::string request_line;
  std::string server_name;
  std::string url;
  std::string server_port;
  std::string http_type;
  std::string method; // GET, POST, CONNECT

  HttpRequest() { data.resize(MAXSIZE); }

  ~HttpRequest(){};

  void request_setting() {
    std::string buff(data.begin(), data.end());
    size_t pos = buff.find("\r\n");
    request_line = buff.substr(0, pos);

    pos = buff.find(" ");
    method = buff.substr(0, pos);

    if (method == "GET" || method == "POST") {
      server_port = "80";
    } else if (method == "CONNECT") {
      server_port = "443";
    } else {
      std::cerr << "Method can't be realized by our proxy\n" << std::endl;
      return;
    }

    buff.erase(0, pos + 1);

    pos = buff.find(" HTTP");
    url = buff.substr(0, pos);

    buff.erase(0, pos + 1);

    pos = buff.find("\r\n");
    http_type = buff.substr(0, pos);

    buff.erase(0, pos + 2);
    std::string temp = url;

    if (method == "CONNECT") {
      pos = temp.find(":");
      server_name = temp.substr(0, pos);
    } else {
      pos = temp.find("://");
      if (pos != std::string::npos) {
        temp.erase(0, pos + 3);
      } else {
        pos = temp.find(":");
        if (pos != std::string::npos) {
          server_name = temp.substr(0, pos);
          temp.erase(0, pos + 1);

          pos = temp.find("/");
          server_port = temp.substr(0, pos);
        } else {
          pos = temp.find("/");
          if (pos != std::string::npos) {
            server_name = temp.substr(0, pos);
          } else {
            server_name = temp;
          }
        }
      }
    }
  }
};

#endif
