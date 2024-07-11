#ifndef REQUEST_H
#define REQUEST_H

#include "User.h" // Assuming User is correctly defined elsewhere
#include <string>

class Request {
public:
    enum RequestState {
        RequestGenerated,
        RequestSent,
        RequestAgree,
        RequestRefused
    };

    User* fromUser;
    User* toUser;
    RequestState currentState;
    int initiative;
    int stepTime;
public:
    Request(User* from, User* to, Request::RequestState, int initiative, int stepTime);
    Request();
    Request(const Request &other){
        fromUser = other.fromUser;
        toUser = other.toUser;
        currentState = other.currentState;
        initiative = other.initiative;
        stepTime = other.stepTime;
    }

    // Getters
    User* getFromUser() const { return fromUser; }
    User* getToUser() const { return toUser; }
    RequestState getCurrentState() const { return currentState; }
    int getInitiative() const { return initiative; }
    int getStepTime() const { return stepTime; }

    // Setters
    void setFromUser(User* from) { fromUser = from; }
    void setToUser(User* to) { toUser = to; }
    void setCurrentState(RequestState state) { currentState = state; }
    void setStepTime(int stepTime) { initiative = stepTime; }

    static Request* isMatchUserFromUser(const std::vector<Request*>& requestList, User* matchUser, User* requestingUser);
};

#endif // REQUEST_H
