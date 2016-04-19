#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>


#define FILENAME "/var/run/labb2.pid"

int main(int argc,char const *argv[])
{
    FILE *fp;

    if(!fork()){
        //redirect stdout to file and make sure only one pid is written
        if(!(fp=freopen(FILENAME,"wx", stdout))) {              //x-clusive rights prevent several servers
           syslog(LOG_INFO, "unable to open file %s, code %d (%s)",FILENAME, errno, strerror(errno));
           exit(1);
        }
        char *args[]={"sh","-c","/C/statusServer tmp/server",NULL};
        execvp("/bin/sh",args);
        syslog(LOG_ERR, "Exec", strerror(errno));
        exit(1);
    }
    wait(0);
    return 0;
}
