//
//  port_hearts.h
//  
//
//  Created by Johan Lipecki on 2016-04-20.
//
//

#ifndef port_hearts_h
#define port_hearts_h

#define SYN0 "hearts"
#define ACK0 "diamonds"
#define SYN1 "port"

#define GAME_SERVER "./game_server"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define PARENT 1

/* Change this to whatever your daemon is called */
#define DAEMON_NAME "Hearts_SYN-ACK"

/* Change this to the user under which to run */
#define RUN_AS_USER "grupp 7"

#define FILENAME "/var/tmp/hearts_syn-ack.pid"
#define GAMELOG "/var/tmp/hearts_log_"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define IP_ADDRESS "130.237.84.89"
#define PORT "1337"

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <pwd.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
//#include "account.h"

// Generates a 32 char length (33 with \0) GUID and stores it in "guid"
int getGuid(char guid[]);
//takes a lockfile pointer as argument and daemonizes process
void daemonize(const char *);
// Forks a game server if provided with a port number,
// returns what system(3) returns, -1 indicates an error.
int start_game_server(int);
// Checks string for SYN0 if second argument points to a 0 and checks for SYN1 if second argument doesn't point to a 0
// Using the socket descriptor in the third argument, ACK0 is returned to the socket for SYN0
// arguments 4 and 5 are returned to the socket for SYN1
int syn_ack(char *,int*,int,int,int);
// Returns a random number between 40 k and 50 k
int get_random_port_number(void);
//Asks user for Account username and password over given TCP - socket descriptor.
// stdout is closed after this function. Use freopen() to retreive it again.
// Returns Account with username and password.
Account prompt_for_login(int*);
int getGuid(char[]);
//Removes lockfile for another to take it's place
int kill_server(void);
void sigchld_handlr(int);
void sigchld_handler(int);
void child_handler(int);
#endif /* port_hearts_h */
