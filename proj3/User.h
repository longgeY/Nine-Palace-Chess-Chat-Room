#ifndef USER_H
#define USER_H

#include "Email.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <locale>
#include <cctype>
class User {

public:

    enum State {
        Idle,
        LoggedIn,
        AwaitingMatch,
        MatchAcceptance, // The state we're focused on
        InMatch,
        Observation,
        // Add other states as needed
    };

    std::string username; 
    std::string password;
    int id;
    int sockId;
    std::string information; 
    bool quiet;
    bool login; // whether this user is online
    std::string observeId;
    std::string playId;
	float rating;
	int win;
	int loss;
    int cmd;
    std::vector<std::string> blocked_names;
    User::State currentState;
    int currentGameID;
    std::vector<Email*> emails;
    Email* current_email;
    vector<int> obGameID;

    static std::vector<User*> allUsers;
    static User* findUser(const std::string& username);
    void logout();
    void writef(std::string buf);

    // Getters
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getInformation() const;
    std::vector<std::string> getBlockedNames() const;
    bool isQuiet() const;
    bool isLogin() const;
    std::string getObserveId() const;
    std::string getPlayId() const;
    int getSockId() const;
    float getRating() const;
    int getWin() const;
    int getLoss() const;
    int getId() const;
    int getcmd() const;
    State getState() const;
    int getCurrentGameID() const;

    // Setters
    void setUsername(const std::string& val);
    void setPassword(const std::string& val);
    void setInformation(const std::string& val);
    void setBlockedNames(const std::vector<std::string>& val);
    void setQuiet(bool val);
    void setLogin(bool val);
    void setObserveId(const std::string& val);
    void setPlayId(const std::string& val);
    void setSockId(int val);
    void setRating(float val);
    void setWin(int val);
    void setLoss(int val);
    void setId(int val);
    void setcmd(int cmd);
    void setState(State newState);
    void setCurrentGameID(int val);
    void win1();
    void loss1();

    User(std::string username, std::string password);
	User();
    User(const User &other){
        username = other.username;
        password = other.password;
        id = other.id;
        sockId = other.sockId;
        information = other.information;
        quiet = other.quiet;
        login = other.login;
        observeId = other.observeId;
        playId = other.playId;
        rating = other.rating;
        win = other.win;
        loss = other.loss;
        cmd = other.cmd;
        blocked_names = other.blocked_names;
    }
};


#endif // USER_H