/*
** echos.c -- the echo server for echoc.c; demonstrates unix sockets
*/

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

/* Change this to whatever your daemon is called */
#define DAEMON_NAME "Anakin"

/* Change this to the user under which to run */
#define RUN_AS_USER "johan"

#define SOCKET "/tmp/server"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void sigchld_handlr(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}

void sigchld_handler(int s)
{
    switch(s) {
        case(SIGTSTP):  exit((int)SIG_IGN);  break;/* Various TTY signals */
        case(SIGTTOU):  exit((int)SIG_IGN);  break;
        case(SIGTTIN):  exit((int)SIG_IGN);  break;
        case(SIGHUP):   exit((int)SIG_IGN);  break;/* Ignore hangup signal */
        default:        exit((int)SIG_DFL);  break;
    }
}

void child_handler(int signum)
{
    switch(signum) {
        case SIGALRM:   exit(EXIT_FAILURE); break;
        case SIGUSR1:   exit(EXIT_SUCCESS); break;
        case SIGCHLD:   exit(EXIT_FAILURE); break;
        default:        exit((int)SIG_DFL); break;
    }
}

void daemonize(const char *lockfile)
{
    pid_t pid, sid, parent,child;
    int lfp = -1;
    struct sigaction act,old_act,act2;

    act.sa_handler=child_handler;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;

    /* already a daemon */
    if (getppid() == 1 ) {
        printf("already a daemon!\n");
        return;
    }
    //printf("Ready for the lockfile!\n");
    /* Create the lock file as the current user */
    if ( lockfile && lockfile[0] ) {
        lfp = open(lockfile,O_RDWR|O_CREAT,0640);   //|O_EXCL taken care of in the start-file
        if ( lfp < 0 ) {
            syslog( LOG_ERR, "unable to create lock file %s, code=%d (%s)",
                    lockfile, errno, strerror(errno) );
            exit(EXIT_FAILURE);
        }
    }
    //printf("lockfile done\n");

    /* Drop user if there is one, and we were run as root */
    if ( getuid() == 0 || geteuid() == 0 ) {
        struct passwd *pw = getpwnam(RUN_AS_USER);
        if ( pw ) {
            syslog( LOG_NOTICE, "setting user to " RUN_AS_USER );
            setuid( pw->pw_uid );
        }
    }

    /*
    Trap signals that we expect to recieve,  sighandler_t signal(int signum, sighandler_t handler)
    http://www.gnu.org/software/libc/manual/html_node/Sigaction-Function-Example.html
    */
    sigaction(SIGCHLD,&act,&old_act);
    sigaction(SIGUSR1,&act,&old_act);
    sigaction(SIGALRM,&act,&old_act);

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        syslog( LOG_ERR, "unable to fork daemon, code=%d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {

        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
        alarm(2);
        pause();

        exit(EXIT_FAILURE);
    }

    /* At this point we are executing as the child process */
    printf("%d\n",child = getpid());        //this writes the pid to revd.pid and ensures all other data is wiped
    freopen("dev/null", "w", stdout);

    /* Cancel certain signals */
    sigaction(SIGCHLD,NULL,&old_act); /* A child process dies */
    sigaction(SIGTSTP,&act2,NULL); /* Various TTY signals */
    sigaction(SIGTTOU,&act2,NULL);
    sigaction(SIGTTIN,&act2,NULL);
    sigaction(SIGHUP,&act2,NULL);
    sigaction(SIGTERM,&act2,NULL);

    /* Change the file mode mask */
    //printf("umask!\n");
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        syslog( LOG_ERR, "unable to create a new session, code %d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        syslog( LOG_ERR, "unable to change directory to %s, code %d (%s)",
                "/", errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
    //printf("directory changed\n");

    /* Redirect standard files to /dev/null */
    freopen("dev/null", "r", stdin);       //stdin is redirected to the socket
    freopen("dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    /* Tell the parent process that we are A-okay */
    kill(parent, SIGUSR1 );
}

int spawn (char* arguments,int fd){ // const char *argv[]){
	pid_t child_pid;
	int i,j,n;
	if(arguments[0]=='c' && arguments[1]=='d'){             //Change directory?
	    //Shift string over to path input
        for(n=3,i=0;i<n;i++){
            int j=0;
            do{
                arguments[j]=arguments[j+1];
                j++;
            } while(arguments[j-1]!='\0');  //stop when '\0' has been shifted over
        }
        close(1);
        dup(fd);
        if((chdir(arguments))!=0) {
            syslog(LOG_ERR,strerror(errno));
            strcpy(arguments,"echo No such directory or file"); //No such directory
            spawn(arguments,fd);
        }
    }
    else {
        /* Duplicate this process. */
        child_pid = fork ();
        if (child_pid != 0){
        /* This is the parent process. */
            close(1);
            wait(0);
            return 0;
        }

        else {
            close(1);
            dup(fd);
            /* Now execute the commands in a new session*/
            execlp("/bin/sh","bash","-c", arguments, NULL);
            /* The execvp function returns only if an error occurs. */
            perror("Exec\n");
            abort ();
        }
    }
}

int main(int argc,char const *argv[])
{
    int done, i, j, len, n, pid, r, s, s2, t;
    struct sockaddr_un local, remote;
    char arguments[100],str[10000];
    memset(arguments,'\0',sizeof(arguments));
    memset(str,'\0',sizeof(str));

    /* Initialize the logging interface */
    openlog(DAEMON_NAME, LOG_PID, LOG_LOCAL5 );
    syslog(LOG_INFO, "starting" );

    /* One may wish to process command line arguments here */

    /* Daemonize */
    daemonize( "/var/lock/" DAEMON_NAME);
    syslog(LOG_INFO, "We did get to daemonize!\n");

    /* Now we are a daemon -- do the work for which we were paid */

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    } else syslog(LOG_INFO, argv[1]);

    local.sun_family = AF_UNIX;
    (strcpy(local.sun_path, argv[1]));
    syslog(LOG_INFO, "path copied!\n");

    if(unlink(local.sun_path)){
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    } else syslog(LOG_INFO, "path unlinked!\n");

    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    } else syslog(LOG_INFO, "bound!\n");

    if (listen(s, 5) == -1) {
        syslog(LOG_ERR, strerror(errno));
        exit(1);
    }
    syslog(LOG_INFO, "listening!\n");

    for(;;) {
        struct sigaction sa;
        sa.sa_handler = sigchld_handlr; // reap all dead processes
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            syslog(LOG_ERR, strerror(errno));
            exit(1);
        }
        syslog(LOG_INFO, "Waiting for a connection...\n");
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
          syslog(LOG_ERR, strerror(errno));
          exit(1);
        }
        /* fork efter acceptet! */
        if((pid=fork())==-1){               // fork och felhantering
            syslog(LOG_ERR, strerror(errno));
            exit(1);
        }
        else if(!pid){                              //serverbarnet ärver accepten, socketen och fildeskriptorn.
            //printf("Connected.\n");

            done = 0;
            do {
                r = recv(s2, arguments,100, 0);
                if (r <= 0) {
                    if (r < 0) perror("recv");
                    done = 1;                                   //försäkrar oss om att accept-loopen avslutas nedan ...
                }                                               //om recv returnerar 0 eller -1.
                if (!done){                                     //Inget fel eller avslut, enligt tilldelning
                        if(!(spawn (arguments,s2))){
                            strcpy(arguments,"ENDOFTRANS");
                            if (send(s2,arguments,strlen(arguments),0) < 0) {  //skicka tillbaka strängen
                                perror("send");
                                done = 1;                   //försäkrar oss om att accept-loopen avslutas
                            }
                            else done = 0;
                        }
                        //memset(arguments,'\0',sizeof(arguments));
                }
            } while (!done);                        //så länge klienten skickar data håller vi öppet 24/7
            printf("I'm server %d and my client just signed off!\n",getpid());
            syslog(LOG_NOTICE, "terminated" );
            closelog();
            exit(0);
        }
        else close(s2);
        //if((wate=waitpid(0,NULL,WNOHANG))==-1)perror("waitpid\n");
    }
    /* Finish up */
    syslog(LOG_NOTICE, "terminated" );
    closelog();
    return 0;
}



