//
//  players.c
//  Test
//
//  Created by Johan Lipecki on 2016-04-15.
//  Copyright © 2016 Johan Lipecki. All rights reserved.
//

#include "players.h"


void* player_waits_or_plays (void *arguments) {
        Player *args = (Player *) arguments;
        UDPsocket udPsocket;
        while (1) {
                int my_turn = 1;
                if (my_turn) {
                        if (!(SDLNet_UDP_Send(args->udpArguments->udPsocket,
                                              args->udpArguments->channel,
                                              &(args->udpArguments->address)))){
                                syslog(LOG_ERR, "%s", strerror(errno));
                                my_turn = 0;
                        }
                        else {
                                // try to receive a waiting udp packet
                                // UDPsocket udpsock;
                                UDPpacket *packet=args->udpArguments->udPpacket;
                                udPsocket=args->udpArguments->udPsocket;
                                int numrecv;
                                numrecv = SDLNet_UDP_Recv(udPsocket, packet);
                                if (numrecv) {
                                        // do something with packet
                                }
                        }
                }
                else {
                        sleep(15);
                }
                UDPpacket mottaget_paket;
                SDLNet_UDP_Recv(udPsocket, &mottaget_paket);
                char *trick[]={"FF;FF;FF;FF;"};

                //klientens angivna position uppdaterar handen med kortet som skickats
                sprintf(trick[0], "%c%c", mottaget_paket.data[1],
                        mottaget_paket.data[2]);
        }
}
UDPpacket createPacket(int cnl, uint8_t *data, int len, int maxlen, int status, IPaddress adr){
        UDPpacket pkt;
        pkt.channel = cnl;
        pkt.data = data;
        pkt.len = len;
        pkt.maxlen = maxlen;
        pkt.status = status;
        pkt.address = adr;
        return pkt;
}

/*void test(int argc,char *argv[])
{
    // receives hearts_start, "%s %s %s %s %s %s", GAME_SERVER, port, ipv4 array
    Trick *trick1;
    IPaddress iPaddress;

    if (init_net()) printf("Success on init\n");
    __uint16_t port = htons((__uint16_t) argv[1]);
    __uint32_t ipv4 = htonl((__uint32_t)IP_ADDRESS);

    iPaddress.host = ipv4;
    iPaddress.port = port;
    trick1->address = iPaddress;

    char *trick[] = {"02","00","2A","1C"};
    for(int i=0;i<4;i++) strcpy(trick1->trick[i],trick[i]);
    // Bind address to the first free channel
    // UDPsocket udpsock;
    // IPaddress *address;
    int chanL=0, mottagna_paket=0;
    char str[40] = {'\0'};
    sprintf(str,"%s;%s;%s;%s;",trick1->trick[0],trick1->trick[1],trick1->trick[2],trick1->trick[3]);

    // create a UDPsocket on port
    UDPsocket udPsocket;

    if(!(udPsocket=SDLNet_UDP_Open(iPaddress.port))){
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(2);
    }
    uint8_t *string=0;
    strcpy(string,str);
    UDPpacket skicka_hand = createPacket(chanL,string,sizeof(str),100,0,iPaddress);
    UDPpacket mottaget_paket;

    if ((chanL = SDLNet_UDP_Bind(udPsocket, -1, &iPaddress)) < 0) {
        syslog(LOG_ERR, "SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        // do something because we failed to bind
    }

}*/
/*
int main(){

    pthread_t player[4]; //Some sort of array of players is needed
    Player control_player[4];
    int i=0;
    int lock;
    int round = 0;

     2. Start simulation by starting the phil-threads and let the main program
     print out the contents of the string table declared above. No thread is going
     to communicate with the user but through the string table, it is the main
     program that prints out the contents of the string table. This means that
     we are separating the task of computation/simulation from the task of
     presentation of the results*/
/*

    for(;i<4;i++) pthread_create(&player[i],NULL,player_waits_or_plays,control_player);
    while(round<13)
    {
        printf("Round %2d: %s\n", round+1, hand);
        sleep(1);
        round++;
    }
    /* The above loop runs in parallel to the threads/phils that affect the
     common resource table.
     IMPORTANT: The synchronization must not be through one mutex! We must have
     one mutex per chopstick, that means an array of mutexes is also needed!
     IMPORTANT: Remember that the program should also make deadlock possible
     through commandline arguments, there must be a way to force a deadlock to
     occur. Remember in this context that all thread-functions are to be based on
     one function, philosophize(), and that this function is the same for all
     threads.
     Of course it can behave differently for different philosopher-id's, but
     it must be one function which needs to be written to enable a forced deadlock
     so we can compare and understand why it normally avoids deadlock.
     3. When the loop has finished, all the threads are joined to the main program
     and then the main program exits.
     return 0;
}
*/



