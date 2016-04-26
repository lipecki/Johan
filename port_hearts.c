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
#include <time.h>
#include "port_hearts.h"

#define PARENT 1

int static get_random_port_numer(void);
int start_game_server(int players){
     /* forka processen när spelare 0 ansluter*/
    int static port;
    if(!players){
        port=get_random_port_numer();
        if(!(fork())){
            //Executing as child process
            execlp("bin/sh","sh","-c",GAME_SERVER,port,NULL);
            syslog(LOG_ERR,"%s",strerror(errno));
        }
        else {
            return port;
            //no waiting around for child? zombie creator?
        }
    }
}
int static get_random_port_numer(){
    srandom(time(NULL));
    return (random()%10000 + 40000);
}
int syn_ack(char* arguments,int syn,int fd){
    pid_t child_pid;
    int static port;
    /* Duplicate this process. */
    child_pid = fork ();
    if(child_pid != 0){
        /* This is the parent process. */
        close(1);
        wait(0);
    }
    
    else {
        //Redirect stdout to socket
        close(1);
        dup(fd);
        //SYN-ACK switch
        switch (arguments) {
            case SYN0:
                if(!syn) arguments=ACK0;
                break;
            case SYN1:
                if(syn==1) return 0;
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
    return PARENT;
}

