/*
** Klient för autentisering av användare ** Prototyp
*/

#include <stdio.h>
#include <unistd.h>
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

#define CLIENT_NAME "Thomas"

#define IP_ADDRESS "130.237.84.89"

int main(int argc,char const *argv[])
{
    int a, i, j, n, s, t, len;
    struct sockaddr_in inet;
    char arguments[100], str[10000];

    /* Initialize the logging interface */
    openlog(CLIENT_NAME, LOG_PID, LOG_LOCAL5 );
    syslog(LOG_INFO, "starting" );

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        syslog(LOG_ERR,"%s",strerror(errno));
        exit(1);
    }
    printf("Trying to connect...\n");
    inet.sin_family = AF_INET;
    inet.sin_port = htons(atoi(argv[1]));
    if ((inet_pton(AF_INET,IP_ADDRESS,&inet.sin_addr))==-1){
        syslog(LOG_ERR, "%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (connect(s, (struct sockaddr *)&inet, sizeof(inet)) == -1) {
        syslog(LOG_ERR,"%s",strerror(errno));
        exit(1);
    }
    printf("Connected.\n");   //Doesn't guarantee someone is listening, only that the socket is being used
    memset(arguments,'\0',sizeof(arguments));
    //om strängen inte är "ENDOFTRANS" från endoftrans()!!)


    for(i=0;i<2;i++){
        switch (i) {
            case 0:
                arguments=SYN0;
                break;
            case 1:
                arguments=SYN1;
                break;
            default:
                syslog(LOG_ERR,"SYN-ACK");
                break;
        }
        if (send(s, arguments,sizeof(arguments), 0) == -1) {
            syslog(LOG_ERR,"%s",strerror(errno));
            exit(1);
        }
         //om strängen inte är "ENDOFTRANS"  {
        while((t=recv(s, str, 100, 0)) > 0) {
            str[t] = '\0';
            if(!(strcmp("ENDOFTRANS",str))){
                printf("That's all folks!");
            }
            else printf("%s", str);
        }
        if(t < 0){
            perror("recv");
            exit(1);
        } else if (t==0) printf("Server closed connection\n");
    }
    close(s);
    closelog();
    return 0;
}



