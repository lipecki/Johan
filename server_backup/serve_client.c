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
#include "game.h"
#include "players.h"

#define BUFLEN 512
#define NPACK 4
#define GAMEPORT 41337

void diep(char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}
// from UDP made simple at https://www.abc.se/~m6695/udp.html
int main(int argc, char *argv[]) {
	

	struct sockaddr_in si_me, si_oth,si_other[4];
	int s, sockFd[4];
	socklen_t slen=sizeof(struct sockaddr_in);
	ssize_t len;
	FILE *fd;

	fd = fopen("/var/tmp/serve_client","w");
	char buf[BUFLEN]={"start"};
	char *trick[4];
	FF_trick(trick);

	//Game initiation, shuffling and dealing.
	Card sorted_deck[52];
	Card shuffled_deck[52];
	char *deck[52];
	new_deck(sorted_deck);
	shuffle_deck(sorted_deck,shuffled_deck);
	convert_card_struct(shuffled_deck,deck,buf);
	Game *game = malloc(sizeof(Game));
	memcpy(game->deck,deck,sizeof(deck));
	for (int k = 0; k < 4; k++) memcpy(game->buffer[k],buf, sizeof(buf));
	for (int k = 0; k < 4; k++) printf("Shuffled deck: \n%s\n\n",game->buffer[k]);

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		diep("socket");
	printf("socket open\n");
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(GAMEPORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, &si_me, sizeof(si_me))==-1)
		diep("bind");
	printf("bound\n");

	//Player initiation
	int counter[4];
	for(int i=0;i<4;i++){
		counter[i] = i;
	}

	pthread_t players[4];
	for(int i=0;i<4;i++){
		pthread_t tmp=0;
		players[i] = tmp;
	}
	//Player thread arguments are initiated
	Player player[4];
	for(int i=0;i<4;i++){
		memset((char *) &si_other[i], 0, sizeof(si_other[i]));
		Player tmp={i,&s,game,&si_me,&si_other[i]};
		memcpy(&player[i], (void *) &tmp, sizeof(tmp));
	}
	//deal cards into each player's game buffer
	for (int k = 0; k < 4; k++) {
		sprintf(player[k].game->buffer[k],"%s;",game->deck[k]);
		for(int j=4;j<52;j+=4) {
			strcat(player[k].game->buffer[k], game->deck[(j+k)]);
			strcat(player[k].game->buffer[k],";");
		}
		printf("Player %d game buffer: %s\n",k,player[k].game->buffer[k]);
	}
	for (int k = 0; k < 4; k++)printf("Player %d game buffer: %s\n",k,player[k].game->buffer[k]);

	//deal cards into player hands
	for (int k = 0; k < 4; k++) {
		for(int j=0;j<52;j+=4) {
			player[k].game->hands[k][j/4] = malloc(3);
			strcpy(player[k].game->hands[k][j/4], game->deck[(j+k)]);
		}
	}
	/* print hands
	for (int k = 0; k < 4; k++) {
		printf("Player %d game hand: ", k);
		for (int l = 0; l < 13; l++) {
			printf("%s ", player[k].game->hands[k][l]);
		}
		printf("\n");
	}*/


	int j=0,connected=0;
	void *buffer = (void *) strdup(buf);
	bool connections[]={false,false,false,false};
	char *this_is_my_pos[4];

	do{
		printf("inet: %s\tsocket descriptor: %d\n\n",
		       inet_ntoa(player[j].si_other->sin_addr),
				 player[j].sockfd);
		if((len = recvfrom(s, buffer, BUFLEN, 0, &si_oth, &slen)) == -1) diep("recvfrom()");
		printf("from addr: %d\n",si_oth.sin_addr.s_addr);

		//check given position
		separate_strings(buffer,";",this_is_my_pos,4);
		if((j=find_DD(this_is_my_pos,4))== -1) printf("not a new connection\n");
		if(j < 4 && j >= 0 && !connections[j]){
			//In preparation for threads
			sockFd[j]= s;
			player[j].sockfd = sockFd[j];
			strcpy(si_other[j].sin_zero,si_oth.sin_zero);
			si_other[j].sin_addr.s_addr=si_oth.sin_addr.s_addr;
			si_other[j].sin_port=si_oth.sin_port;
			si_other[j].sin_family=si_oth.sin_family;

			//Signal that four clients are connected
			connections[j]=true;
			connected += 1;
		} else perror("not a new connection\n");


		printf("Received packet from IP-address: %s: Port: %d\nPosition: %d\nData: %s\nLength: %d\n",
		       inet_ntoa(si_oth.sin_addr), ntohs(si_oth.sin_port),
		       j,
		       (char *) buffer,(int) len);
		//strängen delas upp i fyra
		separate_strings(buffer,";",trick,4);
		printf("received split into: %s",trick[0]);
		for(int i=1;i<4;i++) printf(" %s",trick[i]);

		//skicka första handen
		printf("\nSending packet: \n%s\nto player %d\n", player[j].game->buffer[j],j);
		if (sendto(s, player[j].game->buffer[j], 200, 0, (struct sockaddr *) &si_oth,  slen)==-1)
			diep("sendto()");

	} while (connected<4);

	int starter=0;
	// Här börjar spelet, första handen har skickats till klienterna. Vem börjar?
	starter=who_starts(player[0].game->hands);
	printf("player %d starts\n", starter);
	EE_trick(trick,starter);

	// Lägg nu sticket i fyra buffertar, en till varje spelare (overkill!)
	for (int m = 0; m < 4; m++) {
		for (int k = 0; k < 4; k++) {
			if (!k) sprintf(player[m].game->buffer[m], "%s;", trick[0]);
			else {
				strcat(game->buffer[m], trick[k]);
				strcat(game->buffer[m], ";");
			}
			printf("Game buffer: %s\n", game->buffer[m]);
		}
	}
	for (int k = 0; k < 4; k++)printf("Player %d game buffer: %s\n",k,player[k].game->buffer[k]);

	/*for (int l = 0; l < 4; l) {
		if((len = recvfrom(s, buffer, BUFLEN, 0, (struct sockaddr *) &si_oth, &slen)) == -1) diep("recvfrom()");
	}*/
	if((len = recvfrom(s, buffer, BUFLEN, 0, (struct sockaddr *) &si_oth, &slen)) == -1) diep("recvfrom()");
	printf("from addr: %d\n",si_oth.sin_addr.s_addr);
	for (int l = 0; l < 4; l++) {
		if(si_other[l].sin_addr.s_addr==si_oth.sin_addr.s_addr)
			if (sendto(s, game->buffer[l], sizeof(game->buffer[l]), 0, (struct sockaddr *) &si_other[l],  slen)==-1) diep("sendto()");

	}


	// Jag räknar med att klienten ställer frågor om sticket


	// Förmodligen börjar en do-while-loop här som kör så länge


	// Switch-sats beroende på



	/*while(strcmp(buffer,"quit")){
		printf("buffer: %s \n",player[j].game->buffer);
		if ((len = recvfrom(s, buffer, BUFLEN, 0, &si_oth, &slen)) == -1) diep("recvfrom()");
		printf("Received packet from %s:%d\nData: %s\nLength: %d\n",
		       inet_ntoa(si_oth.sin_addr), ntohs(si_oth.sin_port), (char *) buffer,len);
	*/	//counter[j] = pthread_create(&players[j], NULL, &player_waits_or_plays, (void *) &player[j]);
		//pthread_join((players[j]),NULL);

	printf("\nclient data: %s \n",buffer);
	separate_strings(buffer,";",trick,4);
	printf("received split into: %s ",trick[0]);
	for(int i=1;i<4;i++) printf(" %s",trick[i]);
	printf("\n");
	/*	//Receive data and start game threads for each client
		for(int j=0;j<4;j++) {
			if ((len = recvfrom(player[j].sockfd, player[j].game->buffer, BUFLEN, 0, &si_other[i], &slen)) == -1) diep("recvfrom()");
			counter[j++] = pthread_create(&players[j], NULL, &player_waits_or_plays, (void *) &player[j]);
			printf("Received packet from %s:%d\nData: %s\nLength: %d\n",
			       inet_ntoa(si_other[i].sin_addr), ntohs(si_other[i].sin_port), (char *) buffer,len);
		}

 // Jag försöker föra över detta till trådarna--------------------//
		char cards_to_send[40];
		i=0;
		sprintf(cards_to_send,"%s;",player[i].game->deck[player[i].pos]);
		for(int j=4;j<52;j+=4) {
			strcat(cards_to_send, player[i].game->deck[player[i].pos+j]);
			strcat(cards_to_send,";");
		}
		printf("Sending packet2: \n%s\n", cards_to_send);
		if (sendto(s, cards_to_send, 40, 0, (struct sockaddr *) &si_other[i],  slen)==-1)
			diep("sendto()");
	int k=0;
*/
    	close(s);
    	return 0;
}
