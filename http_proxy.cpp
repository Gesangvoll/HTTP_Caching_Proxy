#include "http_proxy.hpp"
#include "debug.hpp"

using namespace std;

class HttpRequest {
public:
  vector<char> data_buffer;
  string request_line;
  string server_name;
  int server_port;
  string http_type;
  string method;
  int headers_len;
  int content_len;
  int data_len;
};

class HttpResponse {
public:
  vector<char> data_buffer;
  string repsonse_line;
  string status_cde;
  int headers_len;
  int content_len;
  int data_len;

  HttpResponse() : headers_len(0), content_len(0), data_len(0) {}

  ~HttpResponse() {}
};
