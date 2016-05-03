//
//  port_hearts.h
//  
//
//  Created by Johan Lipecki on 2016-04-20.
//
//

#ifndef port_hearts_h
#define port_hearts_h

#define SYN0 "hearts"
#define ACK0 "diamonds"
#define SYN1 "port"

#define GAME_SERVER "./game_server"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define IP_ADDRESS "192.168.0.2"
#define JUNGFRUNS_IP "130.237.84.89"

#include <stdio.h>
int start_game_server(int,int); //
int syn_ack(char *,int,int);
int get_random_port_number(void);

#endif /* port_hearts_h */