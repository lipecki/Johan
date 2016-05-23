/* ** Syn-Ack-Server för projektkursen Programvaruteknik, KTH Vt -16 ** Johan Lipecki, 2016-05-04 */

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
#include "port_hearts.h"
#include "account.h"

int main(int argc,char const *argv[])
{
    int len, n, pid, s, s2, inet_fd, connections=0;
    socklen_t t;
    struct sockaddr_in inet, inet2;
    struct hostent* hostinfo;
    char arguments[100],hearts_start[200];
    char *all_guids[24];
    memset(arguments,'\0',sizeof(arguments));
    memset(hearts_start,'\0',sizeof(hearts_start));

    /* Initialize the logging interface */
    openlog(DAEMON_NAME, LOG_PID, LOG_LOCAL5 );
    syslog(LOG_INFO, "starting" );

    /* Daemonize */
    daemonize( "/var/lock/" DAEMON_NAME);
    syslog(LOG_INFO, "We did get to daemonize!\n");

    /* Now we are a daemon -- do the work for which we were paid */
    // Example from Beej's Guide to Network Programming:
    ////https://beej.us/guide/bgnet/output/html/multipage/sockaddr_inman.html

    if ((inet_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(1);
    } else syslog(LOG_INFO, IP_ADDRESS);

    inet.sin_family = AF_INET;
    inet.sin_port = htons(atoi(PORT));
    inet_pton(AF_INET,IP_ADDRESS,&inet.sin_addr);

    if (bind(inet_fd, (struct sockaddr *)&inet, sizeof inet) == -1) {
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    } else syslog(LOG_INFO, "socket bound!\n");

    if (listen(inet_fd, 8) == -1) {
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    }
    syslog(LOG_INFO, "listening for up to 8 connections!\n");

    for(connections=0;;connections++) {
        //signalhantering, kunde kanske vara utanför loopen men varför ändra ett vinnande koncept?
        int connection_no = (connections%4);
        FILE *log_fp;
        char gamelog[40];
        int port;

        struct sigaction sa = malloc(sizeof(struct sigaction));
        sa.sa_handler = sigchld_handlr; // reap all dead processes
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {      //WNOHANG!
            syslog(LOG_ERR, strerror(errno));
            exit(1);
        }
        // logga spel-pid och guid i spel-logg
        if(!connection_no) {
            (gamelog, "%s_%d", GAMELOG, port=get_random_port_number());
            if ((log_fp = fopen(gamelog, "w")) == NULL) syslog(LOG_ERR,"%s", strerror(errno));
            else {
                fprintf(log_fp, "%d", port);
                fclose(log_fp);
            }
            log_fp = fopen(gamelog, "a+");
        }
        syslog(LOG_INFO, "Waiting for a connection...\n");
        t = sizeof(inet2);
        if ((s2 = accept(inet_fd, (struct sockaddr *)&inet2, &t)) == -1) {
          syslog(LOG_ERR, strerror(errno));
          exit(1);
        }
        /* fork efter acceptet! */
        if((pid=fork())==-1){                                   // fork och felhantering
            syslog(LOG_ERR, strerror(errno));
            exit(1);
        }
        else if(!pid){                                          //serverbarnet ärver accepten, socketen och fildeskriptorn.
            syslog(LOG_INFO,"Connected.\n");
            int i = 0,j=0, done = 0;
            ssize_t r;
            do {
                syslog(LOG_INFO,"About to receive!");
                r = recv(s2, arguments,100, 0);
                if (0 >= r) {
                    if (r < 0) perror("recv");
			done = 1;                                   //försäkrar oss om att accept-loopen avslutas nedan ...
                }                                               //om recv returnerar 0 eller -1
		syslog(LOG_INFO, "Received \"%s\"", arguments);
                while(i) {
                    // You get three tries to login
                    syslog(LOG_INFO,"login-loop entered");
                    Account acc, account;
                    account=prompt_for_login(&s2);
                    if(argv[2]){
                        j += 1;
                        if (j==3){ 
                            strcpy(arguments,"login failed");
                            if (send(s2,arguments,30,0) < 0) {
                                syslog(LOG_ERR, "%s", strerror(errno));
                                close(s2);
                                exit(EXIT_FAILURE);
                            }
                        }
                        else continue;
                    } else {
                        if((connection_no) == 3) {
                            if(start_game_server(get_random_port_number()) < 0){
                                syslog(LOG_ERR,"%s",strerror(errno));
                                send(s2,"game failed, try again",sizeof("game failed, try again"),0);
                                syslog(LOG_INFO,"game start failed");
                            }
                        }
                    } break;
                }
                if (!done){                                     //Inget fel eller avslut, enligt tilldelning
                    syslog(LOG_INFO, "!done\n");
                    if(!(syn_ack(arguments,&i,s2,port,connection_no))){
                        strcpy(arguments,"ENDOFTRANS");
                        if (send(s2,arguments,100,0) < 0) {  //meddela att meddelandet är klart
                            syslog(LOG_INFO,"send-fel");
                            syslog(LOG_ERR,"%s",strerror(errno));
                            done = 1;                               // försäkrar oss om att accept-loopen avslutas
                            memset(arguments,'\0',sizeof(arguments));
                            strcpy(arguments,"login");
                        }
                        else done = 0;
                        syslog(LOG_INFO,"Sent ENDOFTRANS");
                        memset(arguments,'\0',sizeof(arguments));
                    }
                }
                i += 1;
            } while (!done);                        //så länge klienten skickar data håller vi öppet 24/7
            printf("I'm server %d and my client just signed off!\n",getpid());
            syslog(LOG_NOTICE, "terminated" );
            closelog();
            exit(0);
        }
        else close(s2);
        kill_server();
    }
}
