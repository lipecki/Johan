#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "SDL2/SDL_net.h"
#include "SDL2/SDL.h"
#include "players.h"

int init_net();

int main(int argc, char *argv[]) {
	// receives hearts_start, "%s %s %s %s %s %s", GAME_SERVER, port, ipv4 array

	if (init_net()) printf("Success on init\n");
	Args *args;
	Player me;
	me.pos = args->pos;
	// Bind address to the first free channel
	// UDPsocket udpsock;
	// IPaddress *address;
	int chanL, mottagna_paket;
	uint8_t speila = 1;
	uint8_t hand_data;
	char *str;
	sprintf(str, "%s;%s;%s;%s;", *args->trick[0], *args->trick[1], *args->trick[2],
		*args->trick[3]);
	hand_data = atoi(str);

	// create a UDPsocket on port 6666 (server)
	UDPsocket udPsocket;

	udPsocket = SDLNet_UDP_Open(args->address.port);
	if (!udPsocket) {
		printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(2);
	}

	UDPpacket spela = createPacket(chanL, &speila, 1, 100, 0, args->address);
	UDPpacket skicka_hand = createPacket(chanL, &hand_data, sizeof(hand_data), 100, 0, args->address);
	UDPpacket mottaget_paket;

	if ((chanL = SDLNet_UDP_Bind(udPsocket, -1, &args->address) < 0)) {
		syslog(LOG_ERR, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
		// do something because we failed to bind
	}

	pthread_t threads[4];
	Args thread_arg[4];
	__uint16_t port = htons((__uint16_t) atoi(argv[1]));
	//__uint32_t ipv4 = htonl((__uint32_t) atoi(ip));
	char *trick[4];
	int i = 0;
	for (i = 0; i < 4; i++) {
		thread_arg[i].pos = i;
		thread_arg[i].address.port = port;
		thread_arg[i].address.host = htons("130.237.84.89");
		strcpy(*thread_arg[i].trick, *trick);
		pthread_create(&threads[i], NULL, player_waits_or_plays, (void *) &thread_arg[i]);
	}
	// send a packet using a UDPsocket, using the packet’s channel as the channel
	//UDPsocket udpsock;
	//UDPpacket *packet;
	int numsent;
	numsent=SDLNet_UDP_Send(udPsocket, skicka_hand.channel, &skicka_hand);
	if(!numsent) {
		printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
		// do something because we failed to send
		// this may just be because no addresses are bound to the channel...
	}
	return 0;
}



int init_net() {
        if (SDLNet_Init() < 0) {
                printf("SDLNet_Init: %s", SDLNet_GetError());
                return 0;
        	}
        else return 1;
	}


