#ifndef PROXYSERVER_H
#define PROXYSERVER_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stddef.h>



#define MAXSIZE  60000

using namespace std;
class HttpRequest{
public:
    vector<char>  data;
    string request_line;
    string server_name;
    string url;
    string ip_address;
    string server_port;
    string http_type;
    string method; //Get, POST, CONNECT
    string expiredtime;
    int id;
    string ip;
    HttpRequest() {
        data.resize(MAXSIZE);
    }

    ~HttpRequest() {};


    void request_setting() {
        string buff(data.begin(), data.end());
        size_t pos =  buff.find("\r\n");
        request_line = buff.substr(0, pos);

        pos = buff.find(" ");
        method = buff.substr(0, pos);

        if(method == "GET" || method == "POST"){
            server_port  = "80";
        }else if(method == "CONNECT"){
            server_port = "443";
        }else{
            cerr << "Method can't be realized by our proxy\n"<< endl;
	    //        exit(-1);
	    return;
        }

        buff.erase(0,pos+1);

        pos = buff.find(" HTTP");
        url = buff.substr(0, pos);

        buff.erase(0, pos+1);

        pos = buff.find("\r\n");
        http_type = buff.substr(0, pos);

        buff.erase(0,pos+2);
        string temp = url;

        if(method == "CONNECT"){
            pos = temp.find(":");
            server_name = temp.substr(0, pos);
        }else {
            pos = temp.find("://");
            if (pos != string::npos) {
                temp.erase(0, pos + 3);
            }
                pos = temp.find(":");
                if (pos != string::npos) {
                    server_name = temp.substr(0, pos);
                    temp.erase(0, pos + 1);

                    pos = temp.find("/");
                    server_port = temp.substr(0, pos);
                } else {
                    pos = temp.find("/");
                    if (pos != string::npos) {
                        server_name = temp.substr(0, pos);
                    } else {
                        server_name = temp;
                    }

                }

        }

    }

};


class client_info{
public:
    int cl_socket;
    int request_id;
    const char* cl_addr;
    string ip;
    client_info() {}
    client_info(int s, int id, const char* ip ){
        this->cl_addr  = ip;
        this->cl_socket = s;
        this->request_id = id;
    }

    ~client_info() {}


};

class HttpResponse {
public:
    vector<char> data_buff;
    std::string response_line;
    int status_code;
    std::string cache_control;
    std::string date;
    std::string content_length;
    std::string connection;
    std::string content_type;
    std::string server;
    std::string etag;
    std::string expiredtime;
    int id;
    size_t data_buff_len;
    HttpResponse() : status_code(0){};

    void parse(std::string &buff) {
        std::size_t pos = buff.find("\r\n");
        if(pos == string::npos){
            cerr << "hhhh" << endl;
            exit(-1);
        }

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
