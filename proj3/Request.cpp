#include "Request.h"

Request::Request(User* from, User* to, Request::RequestState, int initiative, int stepTime) : fromUser(from), toUser(to), currentState(RequestGenerated), initiative(initiative), stepTime(stepTime) {}
Request::Request() : fromUser(nullptr), toUser(nullptr), currentState(RequestGenerated), initiative(1), stepTime(600) {}

Request* Request::isMatchUserFromUser(const std::vector<Request*>& requestList, User* matchUser,  User* requestingUser) {
    for (const auto& request : requestList) {
        if (request->fromUser == matchUser) {
            if (request->toUser == requestingUser) {
            return request; // Found a request with matchUser as fromUser
            }
        }
    }
    return nullptr; // No matching request found
}
