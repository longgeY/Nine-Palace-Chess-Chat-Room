#include "User.h"
#include <cstring>
#include <unistd.h>
#include <chrono>

std::vector<User*> User::allUsers;

// Constructor with parameters
User::User(std::string username, std::string password)
    : username(username), 
    password(password),
    id(-1), // Assuming default id is 0 or an appropriate default value
    sockId(-1), // -1 often used to indicate an invalid socket id
    information(""),
    quiet(false),
    login(false),
    observeId(""),
    playId(""),
    rating(0.0), // Assuming default rating is 0.0 or an appropriate default value
    win(0),
    loss(0),
    cmd(0)
{ 
    
}

// Default constructor
User::User() 
    : sockId(-1) ,
    quiet(false), 
    login(false)
{

}

User* User::findUser(const std::string& username) {
    for (User* u : allUsers) {
        std::cout << "data:" << u->username << " || this:" << username << std::endl;
        if (u->username == username) {
            std::cout << "found\n";
            return u;
        }
    }
    return nullptr;
}

void User::logout() {
    login = false;
    cmd = 0;
    sockId = -1;
    //game-related should be dealed here
}

void User::writef(std::string buf) {
    char outbuf[1024];
    sprintf(outbuf, "<%s: %i> %s", username.c_str(), cmd, buf.c_str());
    cmd++;
    // Ensure num is the length of outbuf, not the result of the read call
    write(sockId, outbuf, strlen(outbuf)); 
}

// Getters
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }
std::string User::getInformation() const { return information; }
std::vector<std::string> User::getBlockedNames() const { return blocked_names; }
bool User::isQuiet() const { return quiet; }
bool User::isLogin() const { return login; }
std::string User::getObserveId() const { return observeId; }
std::string User::getPlayId() const { return playId; }
int User::getSockId() const { return sockId; }
float User::getRating() const { return rating; }
int User::getWin() const { return win; }
int User::getLoss() const { return loss; }
int User::getId() const { return id; }
int User::getcmd() const { return cmd;}
User::State User::getState() const { return currentState; }
int User::getCurrentGameID() const {return currentGameID; }

// Setters
void User::setUsername(const std::string& val) { username = val; }
void User::setPassword(const std::string& val) { password = val; }
void User::setInformation(const std::string& val) { information = val; }
void User::setBlockedNames(const std::vector<std::string>& val) { blocked_names = val; }
void User::setQuiet(bool val) { quiet = val; }
void User::setLogin(bool val) { login = val; }
void User::setObserveId(const std::string& val) { observeId = val; }
void User::setPlayId(const std::string& val) { playId = val; }
void User::setSockId(int val) { sockId = val; }
void User::setRating(float val) { rating = val; }
void User::setWin(int val) { win = val; }
void User::setLoss(int val) { loss = val; }
void User::setId(int val) { id = val; }
void User::setcmd(int val) { cmd = val; }
void User::setState( User::State newState) { currentState = newState; }
void User::setCurrentGameID(int val) { currentGameID = val; }


void User::win1() { 
    win++;
    rating += 0.5;
}

void User::loss1() { 
    loss++;
    rating -= 0.5;
}