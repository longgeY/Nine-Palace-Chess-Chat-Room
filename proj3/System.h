#ifndef SYSTEM_H
#define SYSTEM_H

#include "User.h" // Corrected include path
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include "Request.h"
// #include "GameList.h"
#include "GameRecall.h"

using namespace std;

class System {
public:
    std::vector<User*> allUsers;
    std::vector<User*> onlineUsers;
    std::vector<std::string> getAllUsers();
    void init();
    void who();
    void load_user();
    std::vector<int> states = std::vector<int>(100, 0);

    User* findUser(std::string username);
    User* findUserFd(const int fd);
    void onlineUpdate();
    void who(int fd);
    void regist(int fd, std::string username, std::string password);
    void stats(int fd, const std::string name);
    void writeLine(int socketId, const std::string line);
    static void rtrim(std::string &s);
    void printAllUsers() const;
    void help(int fd);
    void info(int fd, char* buf);
    
    void startAutoSave();
    void stopAutoSave();
    void saveUserData(); 
    void shout(int fd, char* buf);
    void passwd(int fd, char* buf);
    void block(int fd, char* buf);
    void unblock(int fd, char* buf);
    void tell(int fd, char* buf);
    int send_mail_1(int fd, char*buf);
    int send_mail_2(int fd, char*buf);
    void list_mail(int fd, char*buf);
    void read_mail(int fd, char*buf);
    void delete_mail(int fd, char*buf);
    void saveMailData(); 
    void load_mail();
    void match1(int fd, char*buf,vector<GameRecall*> &gameList,vector<Request*> &requestList,Request req);
    void match2(int fd, char*buf,vector<GameRecall*> &gameList,vector<Request*> &requestList);
    void game(int fd, char*buf,vector<GameRecall*> gameList);
    void kibitz(int fd, char*buf);
    void observe(int fd, char*buf, vector<GameRecall*> &gameList);
    void unobserve(int fd, char*buf, vector<GameRecall*> &gameList);
    void Refresh(int fd, vector<GameRecall*> &gameList);
    void admitDefeat(int fd, vector<GameRecall*> &gameList);
    void kibitz(int fd, char*buf, vector<GameRecall*> &gameList);

    // std::atomic<bool> keepRunning;
    // std::thread autoSaveThread;
};

#endif // SYSTEM_H