#include "log.h"
using namespace std;
mutex mutex_lock;
char *gettime() {
    time_t now = time(0);
    char *dt = ctime(&now);
    return dt;
}

void log_request(HttpRequest &request, fstream &fs) {
    lock_guard<mutex> lock(mutex_lock);
    char *curtime = gettime();
    //    cout << "dfdffdsfsdfdffdfdffdfdfdsfd" << endl;
    fs << request.id << ": " << "\"" << request.request_line << "\"" << " from " << request.ip
       << " @ " << curtime << endl;
    fs.flush();
}


void log_requesting(HttpRequest &request, fstream &fs) {
    lock_guard<mutex> lock(mutex_lock);
    fs << request.id << ": Requesting " << "\"" << request.request_line << "\"" << " from "
       << request.request_line << endl;
    fs.flush();
}

void log_responding(HttpResponse &response, fstream &fs) {
    lock_guard<mutex> lock(mutex_lock);
    fs << response.id << ": Responding " << "\"" << response.response_line << "\"" <<endl;
    fs.flush();
}

void log_received_response(HttpResponse &response, fstream &fs) {
    lock_guard<mutex> lock(mutex_lock);
    fs << response.id << ": Received " << "\"" << response.response_line << "\"" <<" from "
       << response.server << endl;
    fs.flush();
}



