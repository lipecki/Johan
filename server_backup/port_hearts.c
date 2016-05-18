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
    char static hearts_start[200];
    sprintf(hearts_start, "%s %d", GAME_SERVER, port);
    return system(hearts_start);
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
        
        //skicka tillbaka strängen
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
Account prompt_for_login(int *socketDescriptor) {
    	freopen("/var/tmp/hearts_login_log.pid","w",stdout);
	syslog(LOG_INFO, "Prompting for login");
    	Account account;
    	char arguments[100];
    	char *account_values[10];
    	account.username = malloc(100);
    	strcpy(account.username, "");
    	if (0 > send(*socketDescriptor, "account", 8, 0)) {
        	syslog(LOG_ERR, "%s", strerror(errno));
        	return account;
    	}
    	if (0 > recv(*socketDescriptor, arguments, 101, 0)) {
        	syslog(LOG_ERR, "%s", strerror(errno));
        	return account;
    	}
    	separate_strings(arguments, ";",account_values, 10);
    	fprintf(stdout,"Given username: %s", strcpy(account.username,account_values[0]));
    	strcpy(account.password,account_values[1]);
	// jag har tagit bort det som stog här och returnerar bara ett konto
	// krypteringen saknas, så förmodligen funkar inte detta!
    	return getAccountByUsername(account_values[0]);
}

