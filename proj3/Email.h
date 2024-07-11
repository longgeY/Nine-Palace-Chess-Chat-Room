#ifndef EMAIL_H
#define EMAIL_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

class Email {
public:
    string send_name;
    string rec_name;
    string tittle;
    string content;
    bool read;
    time_t send_time;

    Email(string send_name, string rec_name, string tittle, string content);
    Email();
};

#endif
