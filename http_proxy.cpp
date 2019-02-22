#include "http_proxy.hpp"
#include "debug.hpp"

using namespace std;

class HttpRequest {
public:
  vector<char> data;
  string line;
  string server_name;
  int server_port;
  string http_type;
  string method;
  int headers_len;
  int content_len;
  int data_len;
};
