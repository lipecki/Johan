/*
** echoc.c -- the echo client for echos.c; demonstrates unix sockets
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


int main(int argc,char const *argv[])
{
    int a, i, j, n, s, t, len;
    struct sockaddr_un remote;
    char arguments[100], str[10000];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    printf("Trying to connect...\n");
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path,argv[1]);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }
    printf("Connected.\n");   //Doesn't guarantee someone is listening, only that the socket is being used
    memset(arguments,'\0',sizeof(arguments));
    //om strängen inte är "ENDOFTRANS" från endoftrans()!!)


    while(printf("Ange kommando > ") && fgets(arguments,sizeof(arguments),stdin)) {
        arguments[strlen(arguments)-1]='\0';
        if (send(s, arguments,sizeof(arguments), 0) == -1) {
          perror("send");
          exit(1);
        }
         //om strängen inte är "ENDOFTRANS"  {
        while((t=recv(s, str, 100, 0)) > 0) {
            str[t] = '\0';
            if((strcmp("ENDOFTRANS",str)))printf("%s", str);
            else break;
        }
        if(t < 0){
            perror("recv");
            exit(1);
        } else if (t==0) printf("Server closed connection\n");
    }
  close(s);
  return 0;
}



