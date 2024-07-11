#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <locale>
#include <cctype>
#include "User.h"
#include "System.h"
#include "Request.h"
// #include "GameList.h"
#include "GameRecall.h"

using namespace std;
vector<int> states(100, 0);
// char* guest_username[100];
// char* guest_password[100];
vector<string> guest_username(100);
vector<string> guest_password(100);

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) 
		printf("child %d terminated.\n", pid);
	return ;
}

void print_fd_set(const fd_set *set, int ccc) {
    for (int i = 0; i < ccc; ++i) {
        if (FD_ISSET(i, set)) {
            printf("FD %d is set\n", i);
        }
    }
}

// static inline void rtrim(std::string &s) {
//     s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
//         return !std::isspace(ch);
//     }).base(), s.end());
// }

void writeLine(int socketId, const string line) {
    string temp = line + '\n';
    write(socketId, temp.c_str(), temp.size());
}

std::string readLine(int socketId) {
    std::string line;
    char buffer;
    ssize_t bytesRead;

    // Continuously read one character from the socket
    while ((bytesRead = read(socketId, &buffer, 1)) > 0) {
        // Check for newline character, indicating end of line
        if (buffer == '\n') {
            break; // Exit the loop since we've reached the end of the line
        }
        line += buffer;
    }

    // Check for errors
    if (bytesRead < 0) {
        std::cerr << "Error reading from socket." << std::endl;
    }

    return line; 
}
System sys;
Request req;

void login(int rec_sock, string username, string password) {
	char buf[1024] = "username(guest):";
	// sys.rtrim(username);
	sys.rtrim(password);
	// string dusername = username.substr(0, username.length() - 2);
	// string dpassword = password.substr(0, password.length() - 2);
	User *user = sys.findUser(username);
	if (user == nullptr) {
		sys.writeLine(rec_sock, "Incorrect username");
		states[rec_sock] = 0;
		write(rec_sock, buf, strlen(buf));
	} else if (user->password==password){
		if (user->login==true) {
			states[user->sockId]=-1;
			char q[100];
			strcpy(q,"sorry, your account has been login in other place, enter any button to quit\n");
			write(user->sockId,q,strlen(q));
			user->sockId = rec_sock;
			states[rec_sock] = 3;
			user->cmd = 0;
		} else {
			user->login = true;
			user->sockId = rec_sock;
			states[rec_sock] = 3;
		}

		bool hasUnreadEmails = false;
		for (Email* currentEmail : user->emails) {
			if (!currentEmail->read) { // Check for unread emails
				hasUnreadEmails = true;
				break; // Exit the loop as soon as an unread email is found
			}
		}
		
		if (hasUnreadEmails) {
			sys.writeLine(rec_sock, "You have new mail!"); // Notify the user about unread emails
		} else {
			sys.writeLine(rec_sock, "You have no new mail."); // Optionally, inform them they have no new mail
		}
				
	} else {
		writeLine(rec_sock, "Incorrect password");
		cout << "user.password = "<<user->password<<endl;
		cout << "password = "<<password<<endl;
		states[rec_sock] = 0;
		write(rec_sock, buf, strlen(buf));
	}
}

void print_hello(int rec_sock) {
    // Make the buffer static so its lifetime extends beyond the function call.
    // Note: This makes the function non-reentrant and not thread-safe.
    char hello[1024];
	char buf[1024];
    sprintf(hello, 
        "%s%s%s%s%s",
        "********************************************************************\n",
        "You are attempting to log into online tic-tac-toe Server.\n",
        "Please be advised by continuing that you agree to the terms of the\n",
        "Computer Access and Usage Policy of online tic-tac-toe Server.\n\n",
        "********************************************************************\n\n\n");
	if (write(rec_sock, hello, strlen(hello)) < 0) {
		perror("hello error");
	}
	strcpy(buf,"username(guest):");
	write(rec_sock, buf, strlen(buf));
}

void playerQuits(std::vector<GameRecall*> gameList ,User* quitPlayer) {
    for (auto& game : gameList) {
        if (game->player1 == quitPlayer || game->player2 == quitPlayer) {
            User* winner = (game->player1 == quitPlayer) ? game->player2 : game->player1;

			std::cout << winner->getUsername() << " wins because the other player quit the game." << std::endl;
			game->isGameOver = true;
            // Declare the other player as the winner
			std::string winMessage = "Player " + winner->getUsername() + " wins because " + quitPlayer->getUsername() + " quit the game.";
    		writeLine(winner->getSockId(), winMessage); // Or send this message to both players
			game->player1->setState(User::Idle);
   			game->player2->setState(User::Idle);
            
            break;
        }
    }
}

void start_server(char* port) {
	int sockfd, rec_sock;
	socklen_t len;
	vector<int> sock_vector;
	struct sockaddr_in addr, recaddr;
	struct sigaction abc;
	char buf[100];
	fd_set allset, rset;
	int maxfd;
	std::vector<GameRecall*> gameList;
	std::vector<Request*> requestList;

	abc.sa_handler = sig_chld;
	sigemptyset(&abc.sa_mask);
	abc.sa_flags = 0;

	sigaction(SIGCHLD, &abc, NULL);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(": Can't get socket");
		exit(1);
	}

	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)atoi(port));

	if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror(": bind");
		exit(1);
	}

	len = sizeof(addr);
	if (getsockname(sockfd, (struct sockaddr *)&addr, &len) < 0) {
		perror(": can't get name");
		_exit(1);
	}

	if (listen(sockfd, 5) < 0) {
		perror(": bind");
		exit(1);
	}


	FD_ZERO(&allset);
	FD_SET(sockfd, &allset);
	maxfd = sockfd;
	sock_vector.clear();

	while (1) {
		rset = allset;
		select(maxfd+1, &rset, NULL, NULL, NULL);
		// printf("FD_ISSET[%d]\n",FD_ISSET(sockfd, &rset));

		if (FD_ISSET(sockfd, &rset)) {
			/* somebody tries to connect */
			// printf("I am ISSET before accept\n");
			// printf("before ip = %s, port = %d\n", inet_ntoa(recaddr.sin_addr), htons(recaddr.sin_port));
			if ((rec_sock = accept(sockfd, (struct sockaddr *)(&recaddr), &len)) < 0) {
				if (errno == EINTR)
					continue;
				else {
					perror(":accept error");
					exit(1);
				}
			}
			print_hello(rec_sock);
			sock_vector.push_back(rec_sock);
			FD_SET(rec_sock, &allset);
			if (rec_sock > maxfd) maxfd = rec_sock;
		}

		auto itr = sock_vector.begin(); 
		while (itr != sock_vector.end()) {
			int num, fd;
			fd = *itr;
			if (FD_ISSET(fd, &rset)) {
				memset(buf, 0, sizeof(buf));
				num = read(fd, buf, 100);
				printf("current fd:[%i]\n",fd);
				if (num == 0) {
					/* client exits */
					close(fd);
					FD_CLR(fd, &allset);
					User *user = sys.findUserFd(fd);
					if (user != nullptr) {
						user->logout();
					}
					states[fd] = 0;
					itr = sock_vector.erase(itr);
					continue;
				} //quit
				if (states[fd]==-1 || strncmp(buf, "quit", 4) == 0 || strncmp(buf, "exit", 4) == 0) {
					// printf("gogoin: %s\n",buf);
					// printf("Client sent 'quit'. Closing connection.\n");
					printf("close socket [%d]\n",fd);
					writeLine(fd, "Man, see you again!");
					close(fd);
					FD_CLR(fd, &allset);
					User *user = sys.findUserFd(fd);
					if (user != nullptr) {
						user->logout();
					}
					playerQuits(gameList,user);
					states[fd] = 0;
					itr = sock_vector.erase(itr);
					continue;
				} // login-username
				else if (states[fd] == 0) {
					if (strncmp(buf,"guest",5) == 0){
						writeLine(fd, "You can only use 'register username password' as a guest.");
						states[fd] = 2;
					}
					else {
						guest_username[fd] = buf;
						write(fd, "password:", strlen("password:"));
						states[fd] = 1;
					}
				} //login-password 
				else if (states[fd] == 1) {
					guest_password[fd] = buf;
					login(fd,guest_username[fd],guest_password[fd]);
					// states[fd]=3;
				} //register
				else if (states[fd] == 2) {
					if (strncmp(buf,"register",8) != 0) {
						writeLine(fd, "You are not supposed to do this.\nYou can only use 'register username password' as a guest.");
						continue;
					}
					else {
						char *token = strtok(buf, " "); 
						token = strtok(NULL, " ");
						if (token == NULL) {
							writeLine(fd, "You are not supposed to do this.\nYou can only use 'register username password' as a guest.");
							continue;
						}
						else {
							string username = token;
							token = strtok(NULL, " ");
							if (token == NULL) {
								writeLine(fd, "You are not supposed to do this.\nYou can only use 'register username password' as a guest.");
								continue;
							}
							else {
								string password = token;
								sys.regist(fd, username,password);
							}
						}
					}

				} //who
				else if (states[fd] == 3 && strncmp(buf, "who", 3) == 0) {
					sys.who(fd);
				} //stats
				else if (states[fd] == 3 && strncmp(buf,"stats",5) == 0) {
					char *token = strtok(buf, " "); 
					token = strtok(NULL, " ");
					if (token != NULL) {
						sys.stats(fd, token);
					}
					else {
						User *user = sys.findUserFd(fd);
						sys.stats(fd, user->username);
					}
				}
				else if (states[fd] == 3 && ((strncmp(buf, "help", 4) == 0) || (strncmp(buf, "?", 1) == 0))) {
					sys.help(fd);
				}
				else if (states[fd] == 3 && (strncmp(buf, "info", 4) == 0)) {
					sys.info(fd, buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "shout", 5) == 0)) {
					sys.shout(fd, buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "quiet", 5) == 0)) {
					User *user = sys.findUserFd(fd);
					user->quiet = true;
					writeLine(fd,"Enter quiet mode.");
				}
				else if (states[fd] == 3 && (strncmp(buf, "nonquiet", 8) == 0)) {
					User *user = sys.findUserFd(fd);
					user->quiet = false;
					writeLine(fd,"Enter nonquiet mode.");
				}
				else if (states[fd] == 3 && (strncmp(buf, "passwd", 6) == 0)) {
					sys.passwd(fd,buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "block", 5) == 0)) {
					sys.block(fd, buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "unblock", 7) == 0)) {
					sys.unblock(fd, buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "tell", 4) == 0)) {
					sys.tell(fd, buf);
				}
				else if (states[fd] == 3 && (strncmp(buf, "mail", 4) == 0)) {
					if (sys.send_mail_1(fd,buf)) {
						states[fd] = 4;
					}
				}
				else if(states[fd] == 4){
					if (sys.send_mail_2(fd,buf)) {
						states[fd] = 3;
					}			
				}	
				else if (states[fd] == 3 && (strncmp(buf, "listmail", 8) == 0)) {
					sys.list_mail(fd,buf);
				}	
				else if (states[fd] == 3 && (strncmp(buf, "readmail", 8) == 0)) {
					sys.read_mail(fd,buf);
				}	
				else if (states[fd] == 3 && (strncmp(buf, "deletemail", 10) == 0)) {
					sys.delete_mail(fd,buf);
				}
				else if (states[fd] == 3 && strncmp(buf, "game", 4) == 0) {
					sys.game(fd,buf,gameList);
                }
				else if (states[fd] == 3 && strncmp(buf, "match", 5) == 0) {
					sys.match1(fd,buf,gameList,requestList,req);
                }
				else if(states[fd] == 3 && sys.findUserFd(fd)->getState() == User::InMatch && 
					(strncmp(buf, "a", 1) == 0 || strncmp(buf, "b", 1) == 0 ||strncmp(buf, "c", 1) == 0)){
					sys.match2(fd,buf,gameList,requestList);
				}
				else if (states[fd] == 3 && strncmp(buf, "observe", 7) == 0) {
					sys.observe(fd,buf,gameList);
                }
				else if (states[fd] == 3 && strncmp(buf, "unobserve", 9) == 0) {
					sys.unobserve(fd,buf,gameList);
                }
				else if(states[fd] == 3 && sys.findUserFd(fd)->getState() == User::InMatch && (strncmp(buf, "refresh", 7) == 0)){
					sys.Refresh(fd,gameList);
				} 
				else if(states[fd] == 3 && sys.findUserFd(fd)->getState() == User::Observation && (strncmp(buf, "refresh", 7) == 0)){
					sys.Refresh(fd,gameList);
				} 
				else if(states[fd] == 3 && sys.findUserFd(fd)->getState() == User::InMatch && (strncmp(buf, "resign", 6) == 0)){
					sys.admitDefeat(fd,gameList);
				}
				else if (states[fd] == 3 && (strncmp(buf, "kibitz", 6) == 0 || strncmp(buf, "'", 1) == 0 )) {
					sys.kibitz(fd,buf,gameList);
				}
				else
				{
					sys.writeLine(fd,"Command not supoort");
				}
				
				
				// else if (states[fd] == 3 && (strncmp(buf, "kibitz", 5) == 0 || strncmp(buf, "?", 1) == 0 ) && 
				// 	sys.findUserFd(fd)->getState() == User::InMatch) {
				// 	sys.match1(fd,buf,gameList,requestList,req);
                // }
				if (states[fd]==3){
					User *user = sys.findUserFd(fd);
					user->writef("");
				} 
			}
			++itr;
		}

		maxfd = sockfd;
		if (!sock_vector.empty()) {
			maxfd = max(maxfd, *max_element(sock_vector.begin(),
						sock_vector.end()));
		}
	}
}

int main(int argc, char * argv[])
{
	if (argc < 2) {
		printf("Usage: a.out port.\n");
		exit(0);
	}
	sys.init();
	start_server(argv[1]);
	// int a;
	// printf("%i\n",a);
}