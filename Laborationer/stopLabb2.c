#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define FILENAME "/var/run/labb2.pid"

int main(void)
{
    char a[10];
    pid_t pid;
    FILE *fd;
    int link;
    if(!(fd=freopen(FILENAME,"r+",stdin))) {
       syslog( LOG_ERR, "unable to open file, code %d (%s)",
                errno, strerror(errno));
        exit(1);
    }

    while(fgets(a,10,fd)) {                 //a loop is good if several processes need killing
        pid = atoi(a);
        if((kill(pid,SIGTERM))) exit(0);    //Terminate revd while signalling succeeds
        memset(a,'\0',sizeof(a));
    }
    fclose(fd);
    //remove the file so that another one may be created
    if((link=unlink(FILENAME))==-1){
       syslog(LOG_ERR, "unable to unlink file, code %d (%s)",
                errno, strerror(errno));
        exit(1);
    }
    return 0;
}
