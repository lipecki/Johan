//
//  port_hearts.c
//  
//
//  Created by Johan Lipecki on 2016-04-20.
//
//

#include "port_hearts.h"

uint32_t getIP(char *str)
{
	
	return 0;
}

int start_game_server(int port){
        char static start[200];
        sprintf(start,"%s %d", GAME_SERVER, port);
        return system(start);
}
int get_random_port_number(void){
        srandom((unsigned)time(NULL));
        return (random()%10000 + 40000);
}
int syn_ack(char* arguments, int *i,int sd, int port, int connection_no){
        if(strcmp(arguments,"ENDOFTRANS")){
	
	syslog(LOG_INFO, "syn-ack argument %d: %s",*i, arguments);
        
	if (!strcmp(arguments, "hearts") && !(*i)) strcpy(arguments,"diamonds");
        
        else if(!(strcmp(arguments, "port")) && (*i)){
            
            sprintf(arguments, "%d;%d", port, connection_no);
        }
        else strcpy(arguments, "This incident will be reported!");
        
        // Return arguments
        if (send(sd,arguments,100,0) < 0) {
            syslog(LOG_ERR, "syn-ack send error: %s", strerror(errno));
        }
        syslog(LOG_INFO,"Exiting syn-ack()");
    }
    else{
        syslog(LOG_ERR,"ENDOFTRANS");
        return 1;
    }
    return 0;
}
//Asks user for Account username and password over given TCP - socket descriptor.
// stdout is closed after this function. Use freopen() to retreive it again.
Account prompt_for_login(int *socketDescriptor) {
    	FILE *fp;
	static char str[100];
	sprintf(str,"%s%d.pid",GAMELOG, getpid());

	//Redirecting stdout from console to file.
	// The idea is to log user IP-address and username
	// so that an interrupted session can be resumed
	// after matching log info with reconnecting client. This is not yet implemented.
	fp = freopen(str,"w",stdout);
	printf("Prompting for login\n");
    	Account account;
    	char arguments[100];
    	char *account_values[10];
    	account.username = malloc(100);
    	strcpy(account.username, "");
    	if (0 > send(*socketDescriptor, "account", 8, 0)) {
        	printf("%s\n", strerror(errno));
        	return account;
    	}
    	if (0 > recv(*socketDescriptor, arguments, 101, 0)) {
        	printf("%s\n", strerror(errno));
        	return account;
    	}
	//The returned csv-string is split into two arguments.
    	separate_strings(arguments, ";",account_values, 10);

	//Both the system log interface and the session log
    	syslog(LOG_INFO,"Given username: %s", strcpy(account.username,account_values[0]));
	printf("Username given: %s\n",account.username);
    	strcpy(account.password,account_values[1]);
	fclose(fp);
    	return account;
}
void daemonize(const char *lockfile)
{
        pid_t pid, sid, parent,child;
        int lfp = -1;
        FILE *pid_fp;
        struct sigaction act,old_act,act2;

        act.sa_handler=child_handler;
        sigemptyset (&act.sa_mask);
        act.sa_flags = 0;

        /* already a daemon */
        if (getppid() == 1 ) {
                syslog(LOG_ERR, "already a daemon! Code = %d (%s)\n",
                       errno, strerror(errno));
                return;
        }
        //printf("Ready for the lockfile!\n");
        /* Create the lock file as the current user */
        if ( lockfile && lockfile[0] ) {
                lfp = open(lockfile,O_RDWR|O_CREAT,0640);   //|O_EXCL taken care of in the start-file
                if ( lfp < 0 ) {
                        syslog( LOG_ERR, "unable to create lock file %s, code=%d (%s)\n",
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
        if(!(pid_fp = fopen(FILENAME,"w"))){
                syslog(LOG_ERR,"pid_fp",strerror(errno));
                exit(EXIT_FAILURE);
        }

        fprintf(pid_fp,"%d\n",child = getpid()); //this writes the pid to revd.pid and ensures all other data is wiped
        fclose(pid_fp);

        /* Cancel certain signals */
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
        } //else syslog(LOG_INFO, "sid ok!");

        /* Change the current working directory.  This prevents the current
           directory from being locked; hence not being able to remove it. */
        if ((chdir("/")) < 0) {
                syslog( LOG_ERR, "unable to change directory to %s, code %d (%s)",
                        "/", errno, strerror(errno) );
                exit(EXIT_FAILURE);
        }
        //printf("directory changed\n");

        /* Redirect standard files to /dev/null */
        freopen("/dev/null", "r", stdin);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);

        /* Tell the parent process that we are A-okay */
        kill(getppid(),SIGUSR1);
        syslog(LOG_INFO, "parent killed!");
}
void sigchld_handlr(int s)
{
        while(waitpid(-1, NULL, WNOHANG) > 0);
}

void sigchld_handler(int s)
{
        switch(s) {
                case(SIGTSTP):  exit((long)SIG_IGN);  break;/* Various TTY signals */
                case(SIGTTOU):  exit((long)SIG_IGN);  break;
                case(SIGTTIN):  exit((long)SIG_IGN);  break;
                case(SIGHUP):   exit((long)SIG_IGN);  break;/* Ignore hangup signal */
                default:        exit((long)SIG_DFL);  break;
        }
}

void child_handler(int signum)
{

        switch(signum) {
                case SIGALRM:   exit(EXIT_FAILURE); break;
                case SIGUSR1:   syslog(LOG_INFO, "child_handler: SIGUSR1"); exit(EXIT_SUCCESS); break;
                case SIGCHLD:   exit(EXIT_FAILURE); break;
                default:        exit((long)SIG_DFL); break;
        }
}

int kill_server(void)
{
        int link;
        //remove the file so that another one may be created
        if((link=unlink(FILENAME))==-1){
                syslog(LOG_ERR, "unable to unlink file, code %d (%s)",
                       errno, strerror(errno));
                exit(1);
        }
        return 0;
}

