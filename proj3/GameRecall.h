#ifndef GAMERECALL_H
#define GAMERECALL_H

#include "User.h"
#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <chrono>
#include <unistd.h>

class GameRecall {
public:
    User* player1;
    User* player2;
    int gameID;
    bool currentPlayer;
    bool isGameOver;
    std::time_t* start_Time;
    std::time_t* end_Time;
    float checkerboard[3][3]; // Board for the game, assuming it's a game like tic-tac-toe
    std::chrono::steady_clock::time_point currentTurnTime;
    std::chrono::steady_clock::time_point endTurnTime;
    std::chrono::seconds player1TimeLeft{600};
    std::chrono::seconds player2TimeLeft{600};
    int move_step;
    std::vector<User*> observers;

    // Constructor
    GameRecall(User* player1, User* player2, int id);

    // Destructor to properly manage dynamic memory
    ~GameRecall() {
        delete start_Time;
        delete end_Time;
    }

    GameRecall();

    GameRecall(const GameRecall &other){
        player1 = other.player1;
        player2 = other.player2;
        currentPlayer = other.currentPlayer;
        isGameOver = other.isGameOver;
        start_Time = other.start_Time;
        end_Time = other.end_Time;
        gameID = other.gameID;
        checkerboard[3][3] = other.checkerboard[3][3];
        currentTurnTime = other.currentTurnTime;
        player1TimeLeft = other.player1TimeLeft;
        player2TimeLeft = other.player2TimeLeft;
    }

    bool addMove(int player, const std::string& move);
    void printBoard() const;
    void manageGame(int fd, GameRecall *game);
    bool isMoveCommand(const std::string command);
    bool isWin(int bw);
    std::string getBoardAsString() const;
    GameRecall* handleMatchRequest(int fd, User* matchUser, User* requestingUser, int gameID);
    GameRecall* startGame(User* player1, User* player2);
    bool isDraw() const;
    void startTurn();
    void endTurn();
    void endGame(int mod);
    void wrtel(int socketId, const std::string line);
    void playOB();

    std::chrono::seconds getPlayer1TimeLeft() const;
    void setPlayer1TimeLeft(const std::chrono::seconds& time);
    std::chrono::seconds getPlayer2TimeLeft() const;
    void setPlayer2TimeLeft(const std::chrono::seconds& time); 
};

#endif // GAMERECALL_H
