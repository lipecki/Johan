//
//  port_hearts.c
//  
//
//  Created by Johan Lipecki on 2016-04-20.
//
//
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "port_hearts.h"

int static get_random_port_numer(int *);
int start_game_server(int *seed){
    
    /* Öppna en csv-fil som spar IP-nummer för spelarna och porten de ansluter mot
     * Räkna anslutna spelare
     * forka processen när det finns fyra spelare */
    int players,new_table_needed=0;
    int static port;
    FILE static *fp;
    fp=fopen("game_server_counter","r+");
    if((players=fgetc(fp))!=EOF){
        if (players<4) players++;
        else if (players==4) new_table_needed=1;
        else syslog(LOG_ERR,"Too many players!");
    } else (players=1);
    fputc(players,fp)
    
    if(new_table_needed){
        fgetc(fp);
        fputc(0,fp);
        port=get_random_port_numer(seed);
        if(!(fork())){
            //Executing as child process
            execlp("bin/sh","sh","-c",GAME_SERVER,port,NULL);
            syslog(LOG_ERR,"%s",strerror(errno));
        }
        else {
            close(1);
            return port;
            //no waiting around for child? zombie creator?
        }
    }
}
int static get_random_port_numer(int *seed){
    srandom(*seed);
    return (random()%10000 + 40000);
}
int syn_ack(char* arguments,int fd,int i){
    pid_t child_pid;
    int static port;
    /* Duplicate this process. */
    child_pid = fork ();
    if(child_pid != 0){
        /* This is the parent process. */
        close(1);
        wait(0);
        return 0;
    }
    
    else {
        //Redirect stdout to socket
        close(1);
        dup(fd);
        //SYN-ACK switch
        switch (arguments) {
            case SYN0:
                if(!i) arguments=ACK0;
                break;
            case SYN1:
                if(i==1){
                    //svara med portnummer och starta spelservern
                    if(!(port=start_game_server())){
                        syslog(LOG_ERR,"no port assigned to game server");
                        exit(EXIT_FAILURE);
                    }
                    //skicka portnummer till klienten!
                    strlcpy(arguments,itoa(port),7);
                }
                break;
            default:
                arguments="it's the ping of death for you my friend!";
                break;
        }
        /* Now execute the commands in a new session*/
        execlp("/bin/sh","bash","-c", "echo" ,arguments, NULL);
        /* The execlp function returns only if an error occurs. */
        syslog(LOG_ERR,"%s",strerror(errno));
        abort();
    }
    //This never happens!
    return 0;
}

