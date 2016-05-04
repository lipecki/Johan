/*
** echoc.c -- the echo client for echos.c; demonstrates unix sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CLIENT_NAME "Thomas"


int main(int argc,char const *argv[])
{
    int a, i, j, n, s, t, len;
    struct sockaddr_in inet;
    char arguments[100], str[10000];
    memset(str,'\0',strlen(str));

    /* Initialize the logging interface */
    openlog(CLIENT_NAME, LOG_PID, LOG_DAEMON );
    syslog(LOG_INFO, "starting" );

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        syslog(LOG_ERR,"%s",strerror(errno));
        exit(1);
    }
    printf("Trying to connect...\n");
    inet.sin_family = AF_INET;
    inet.sin_port = htons(atoi(argv[1]));
    if ((inet_pton(AF_INET,"192.168.56.101",&inet.sin_addr))==-1){
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //strcpy(remote.sun_path,argv[1]);
    //len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&inet, sizeof(inet)) == -1) {
        syslog(LOG_ERR,"%s",strerror(errno));
        exit(1);
    }
    printf("Connected.\n");   //Doesn't guarantee someone is listening, only that the socket is being used
    memset(arguments,'\0',sizeof(arguments));
    //om strängen inte är "ENDOFTRANS" från endoftrans()!!)


    while(printf("Ange kommando > ") && fgets(arguments,sizeof(arguments),stdin)) {
        arguments[strlen(arguments)-1]='\0';
        printf("Du skrev: %s\n", arguments);
        if (send(s, arguments,sizeof(arguments), 0) == -1) {
            perror("send");
            exit(1);
        }
         //om strängen inte är "ENDOFTRANS"  {
        while((t=recv(s, str, 100, 0)) > 0) {
            	str[t] = '\0';
            	printf("retur: %s\n", (str));
            	if(!(strcmp(str,""))) break;
        }
        if(t < 0){
            perror("recv");
            exit(1);
        } 
	else if(t==0) printf("Server closed connection\n");
 	else {
		printf("%s\n",str);
		memset(str,'\0', sizeof(str));
	}
    }
  close(s);
  return 0;
}



