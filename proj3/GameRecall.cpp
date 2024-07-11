#include "GameRecall.h"
#include "User.h"
#include "System.h"
#include <chrono>
#include <unistd.h>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string>
GameRecall::GameRecall()
{

}

GameRecall::GameRecall(User* player1, User* player2, int id)
    : player1(player1), 
    player2(player2), 
    gameID(id), 
    currentPlayer(false), 
    isGameOver(false)
{
    start_Time = new std::time_t(std::time(nullptr)); // Initialize start time to now
    end_Time = nullptr; 
    std::memset(checkerboard, 0, sizeof(checkerboard));
}

bool GameRecall::addMove(int player, const std::string& move) {
    // startTurn();
    if (move.length() != 2) return false; // Ensure move is exactly 2 characters long

    // Convert 'a', 'b', 'c' to row index 0, 1, 2
    int row = move[0] - 'a';
    // Convert '1', '2', '3' to column index 0, 1, 2
    int col = move[1] - '1';

    // Check for valid row and column
    if (row >= 0 && row < 3 && col >= 0 && col < 3) {
        // Additional check if the position is already taken
        if (checkerboard[row][col] == 0) {
            checkerboard[row][col] = player; // Player 1 or 2
            endTurn();
            move_step++;
            currentPlayer = !currentPlayer; // Switch turn
            return true;
        } else {
            std::cout << "This position is already taken.\n";
        }
    } else {
        std::cout << "Invalid move. Please try again.\n";
    }

    return false;
}

void GameRecall::wrtel(int socketId, const std::string line) {
    std::string temp = line + '\n';
    write(socketId, temp.c_str(), temp.size());
}

void GameRecall::endGame(int mod) {
    System sys;
    if (mod == 0){
        wrtel(player1->getSockId(), "It's a draw.");
        wrtel(player2->getSockId(), "It's a draw.");
        player2->writef("");
        isGameOver = true;
    }
    else if (mod == 1){
        wrtel(player1->getSockId(), "You win!");
        player1->win1();
        wrtel(player2->getSockId(), "You lose.");
        player2->loss1();
        player2->writef("");
        isGameOver = true;
    }
    else if (mod == 2){
        wrtel(player2->getSockId(), "You win!");
        player2->win1();
        wrtel(player1->getSockId(), "You lose.");
        player1->loss1();
        player1->writef("");
        isGameOver = true;
    }
    playOB();
    for ( User* user :observers)
    {
        auto it = std::find(user->obGameID.begin(), user->obGameID.end(), gameID);
        if (it != user->obGameID.end()) {
            sys.writeLine(user->getSockId(),"Observer Over!");
            user->setState(User::Idle);
            user->obGameID.erase(it); // Correctly erase gameID from user->obGameID
        }
    }
    
}

// Print the checkercheckerboard
void GameRecall::printBoard() const {
    // Print the column headers
    std::cout << "  1 2 3\n";
    for (int i = 0; i < 3; ++i) {
        // Print the row label ('a' + row index)
        std::cout << static_cast<char>('a' + i) << ' ';
        for (int j = 0; j < 3; ++j) {
            char symbol = '.';
            if (checkerboard[i][j] == 1) symbol = 'X';
            else if (checkerboard[i][j] == 2) symbol = 'O';
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }
}

bool GameRecall::isMoveCommand(const std::string command) {
    if (command.size() == 2) {
        char ch1 = command[0];
        char ch2 = command[1];
        return (ch1 == 'A' || ch1 == 'B' || ch1 == 'C') && (ch2 == '1' || ch2 == '2' || ch2 == '3');
    }
    return false;
}

bool GameRecall::isWin(int bw) {
    for (int i = 0; i < 3; i++) {
        if (checkerboard[i][0] == checkerboard[i][1] && checkerboard[i][0] == checkerboard[i][2] && checkerboard[i][0] == bw) {
            return true;
        }

        if (checkerboard[0][i] == checkerboard[1][i] && checkerboard[0][i] == checkerboard[2][i] && checkerboard[0][i] == bw) {
            return true;
        }
    }

    if (checkerboard[0][0] == checkerboard[1][1] && checkerboard[0][0] == checkerboard[2][2] && checkerboard[0][0] == bw) {
        return true;
    }

    if (checkerboard[0][2] == checkerboard[1][1] && checkerboard[0][2] == checkerboard[2][0] && checkerboard[0][2] == bw) {
        return true;
    }

    return false;
}

std::string GameRecall::getBoardAsString() const {
    std::ostringstream boardStr;
    boardStr << "\nBlack: " << this->player1->getUsername() << "                      " <<"white: " << this->player2->getUsername();
    boardStr << "\nP1 left: " << player1TimeLeft.count() << "s" << "                " <<"P2 left: " << player2TimeLeft.count() << "s" << "\n";

    // Print the column headers
    boardStr << "  1 2 3\n";
    for (int i = 0; i < 3; ++i) {
        // Print the row label ('a' + row index)
        boardStr << static_cast<char>('a' + i) << ' ';
        for (int j = 0; j < 3; ++j) {
            char symbol = '.';
            if (checkerboard[i][j] == 1) symbol = 'X';
            else if (checkerboard[i][j] == 2) symbol = 'O';
            boardStr << symbol << " ";
        }
        boardStr << "\n"; // Use "\n" instead of std::endl to avoid flushing the buffer
    }
    

    return boardStr.str();
}

bool GameRecall::isDraw() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (checkerboard[i][j] == 0) {
                // Found an empty cell, so it's not a draw
                return false;
            }
        }
    }

    return true;
}

GameRecall* GameRecall::handleMatchRequest(int fd, User* matchUser, User* requestingUser, int gameID) {
    System sys;
    // Assuming we have function to initialize GameRecall with both players
    
    GameRecall *game = new GameRecall(matchUser, requestingUser, gameID);
    game->startTurn();
    game->player1->setCurrentGameID(gameID);
    game->player2->setCurrentGameID(gameID);
    player1 = matchUser;
    player2 = requestingUser;
    return game;
}


GameRecall* GameRecall::startGame(User* player1, User* player2) {
    // Initialize the game instance
    GameRecall *game = new GameRecall(player1, player2, 1);
    std::cout << "start in Player1 is : " << player1->getSockId() << endl;
    std::cout << "start in Player2 is : " << player2->getSockId() << endl;
    // Initial setup if needed
    return game;
}

void GameRecall::startTurn() {
    currentTurnTime = std::chrono::steady_clock::now();
}

void GameRecall::endTurn() {
    System sys;
    endTurnTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(endTurnTime - currentTurnTime);
    std::cout << "elapsed time: " << elapsed.count() << " seconds\n";

    // Deduct elapsed time from the current player's remaining time
    if (!currentPlayer) {
        std::cout << "Player1's remaining time before: " << player1TimeLeft.count() << " seconds\n";
        player1TimeLeft -= elapsed;
        std::cout << "Player1's remaining time after: " << player1TimeLeft.count() << " seconds\n";
        if (player1TimeLeft.count() <= 0) {
            sys.writeLine(player1->getSockId(),"Game timeout! You lose.");
            sys.writeLine(player2->getSockId(),"You win!");
            player1->setState(User::Idle);
			player2->setState(User::Idle);
            return;
        }
    } else {
        player2TimeLeft -= elapsed;
        if (player2TimeLeft.count() <= 0) {
            std::cout << "Player2's remaining time: " << player2TimeLeft.count() << " seconds\n";
            // Player 2 has run out of time
            sys.writeLine(player2->getSockId(),"Game timeout! You lose.");
            sys.writeLine(player1->getSockId(),"You win!");
            player1->setState(User::Idle);
			player2->setState(User::Idle);
            return;
        }
    }

    startTurn();
}

void GameRecall::playOB() {
    for(User* user : observers){
        if (user->login == false){
            continue;
        }
        wrtel(user->sockId,"");
        wrtel(user->sockId,getBoardAsString());
        user->writef("");
    }
}

std::chrono::seconds GameRecall::getPlayer1TimeLeft() const {
    return player1TimeLeft;
}

// Setter for player1TimeLeft
void GameRecall::setPlayer1TimeLeft(const std::chrono::seconds& time) {
    player1TimeLeft = time;
}

// Getter for player2TimeLeft
std::chrono::seconds GameRecall::getPlayer2TimeLeft() const {
    return player2TimeLeft;
}

// Setter for player2TimeLeft
void GameRecall::setPlayer2TimeLeft(const std::chrono::seconds& time) {
    player2TimeLeft = time;
}