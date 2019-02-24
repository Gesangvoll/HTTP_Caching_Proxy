#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP
#include <string>
class HttpResponse {
public:
  std::string data_buff;
  std::string response_line;
  int status_code;
  std::string cache_control;
  std::string date;
  std::string content_length;
  std::string connection;
  std::string content_type;
  std::string server;
  std::string etag;
  HttpResponse() : status_code(0){};

  void parse(std::string &buff) {
    std::size_t pos = buff.find("\r\n");
    std::size_t pos2;
    response_line = buff.substr(0, pos);
    status_code = std::stoi(
        buff.substr(buff.find(" ", 0), buff.find(" ", buff.find(" ", 0) + 1)));
    pos = buff.find("\r\nDate: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      date = buff.substr(pos, pos2 - pos);
    }
    pos = buff.find("\r\nContent-Length: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      content_length = buff.substr(pos, pos2 - pos);
    }
    pos = buff.find("\r\nConnection: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      connection = buff.substr(pos, pos2 - pos);
    }
    pos = buff.find("\r\nContent-Type: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      content_type = buff.substr(pos, pos2 - pos);
    }
    pos = buff.find("\r\nServer: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      server = buff.substr(pos, pos2 - pos);
    }
    pos = buff.find("\r\nEtag: ");
    if (pos != std::string::npos) {
      pos = buff.find(" ", pos) + 1;
      pos2 = buff.find("\r\n", pos);
      etag = buff.substr(pos, pos2 - pos);
    }
  }
};

#endif
