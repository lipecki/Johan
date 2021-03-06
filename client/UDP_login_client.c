#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <SDL_net.h>

#define MESSAGE "# This is the gamelog for the card game Hearts, \n# developed for the network communications course at KTH Haninge, spring term 2016"
#define PORT 41337
#define MAXLEN 1024
//#define IP_ADDRESS "130.237.84.89"
#define IP_ADDRESS "127.0.0.1"
#define GAME_CLIENT "game_client "
#define LOGIN_LOG "udp_log"
#define EXPECTED_RESPONSE "diamonds"
#define USER_DATA "Grupp7;password"
#define SERVER_REPLY "41337;1"

int main(int argc,char *argv[]) {
    //int my_pos= (int) argv[2];
    int my_pos=0;
    char *trick[13];
    char *hand[13];
    char *buffer;
    buffer = malloc(150);
    DD_trick(trick,my_pos);

    //DD_trick(trick,my_pos);
    printf("trick: ");
    for(int i=0;i<4;i++) printf(" %s",trick[i]);
    IPaddress ip;
    UDPsocket sd;
    UDPpacket udPpacket;
    char server_ip[25] = {'\0'}, log_string[40], pid[7];
    udPpacket.data = (Uint8 *) strdup("");
    for(int i=0;i<4;i++) {
        strcat(udPpacket.data, (Uint8 *) trick[i]);
        strcat(udPpacket.data,";");
    }
    printf("\npacket: %s\n", udPpacket.data);
    udPpacket.len = 13;
    int result, len, len2;
    uint16_t port;
    FILE *fd;

    sprintf(pid, ".%d", getpid());
    strcpy(log_string, LOGIN_LOG);
    strcat(log_string, pid);
    printf("%s\n",log_string);
    //fd = fopen(log_string, "w+");
    // fprintf(fd, "%s\n",MESSAGE);
    //fclose(fd);
    strcpy(server_ip, IP_ADDRESS);
    //port = (uint16_t) atoi(argv[1]);
    port = PORT;
    if (SDLNet_ResolveHost(&ip, server_ip, port) < 0) {
        printf( "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    if (!(sd = SDLNet_UDP_Open(0))) {
        printf( "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    fd = fopen(log_string, "a+");
    fprintf(fd,"UDPsocket open\n");
    fclose(fd);

    // värdet

    //IPaddress *address;
    int channel, chanL;
    channel = SDLNet_UDP_Bind(sd, -1, &ip);

    if (channel == -1) {
        fd = fopen(log_string,"a+");
        fprintf(fd,"SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
        fclose(fd);
        // do something because we failed to bind
    }
    else    printf("bound to channel %d\n",channel);
    int i = 0;
    int done=0;
    while (!done){
        len=SDLNet_UDP_Send(sd, channel, &udPpacket);
        if (len< 0) {
            if (i++ > 1) break;
            printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
            chanL = (udPpacket.channel);
            channel = chanL;
            // do something because we failed to send
            // this may just be because no addresses are bound to the channel...
        }
        // create a new UDPpacket to hold 1024 bytes of data
        UDPpacket *packet,*answer;
        packet=SDLNet_AllocPacket(1024);
        answer=SDLNet_AllocPacket(1024);
        if(!packet) {
            printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
            sleep(2);
            continue;

        }
        else {
            // do stuff with this new packet
            result=SDLNet_UDP_Recv(sd, packet);
            for(int ind=0; ind < (strlen((char *) packet->data)); ind++) {
                if ((packet->data[ind] < '0') && (packet->data[ind] > 'z')) packet->data[ind]= 'X';
                if ((packet->data[ind] == '?')) (packet->data[ind] = 'X');
                if ((packet->data[ind] == ',')) (packet->data[ind] = 'X');
            }
            if(result) {

                printf("Received packet data: %s\n", (char *) packet->data);
                strcpy(buffer,(char *) packet->data);
                if(strlen(buffer)==39 && !(strstr(buffer,"FF"))){
                    split(buffer,';',trick);
                    printf("Trickr[12]: %s\n",trick[12]);
                    for (int j = 0; j < 13; j++) {
                        hand[j]=malloc(3);
                        strcpy(hand[j],trick[j]);
                        if (j> 3) trick[j]='\0';
                    }
                    printf("Hand: \t");
                    for (int k = 0; k < 13; k++) {
                        printf("%s ",hand[k]);
                    }
                    printf("\n");
                }
                strcpy(buffer,(char *) packet->data);
                split(buffer,';',trick);
                for (int j = 0; j < 4; j++) {
                    if(!(strcmp(trick[j],"EE"))) {
                        printf("player %d starts\n", j);
                        if(j == my_pos) {
                            strcpy(trick[j], "00");
                            printf("changed trick pos %d to: %s", j, trick[j]);
                            //answer->data = (Uint8 *) strdup("");
                            for (int n = 0; n < 4; n++) {
                                strcat(answer->data, (Uint8 *) trick[n]);
                                strcat(answer->data, ";");
                            }
                            printf("\npacket: %s\n", answer->data);
                            answer->len = 13;
                            memcpy(&udPpacket, answer, sizeof(UDPpacket));
                            printf("överkopierad data: %s\n", udPpacket.data);
                            if ((len = SDLNet_UDP_Send(sd, channel, answer)));// done = 1;

                        }
                        else if ((j + 1) % 4 == my_pos && !(strcmp(trick[my_pos], "FF"))) {
                            strcpy(trick[j],"00");
                            strcpy(trick[my_pos], hand[0]);
                            printf("changed trick pos %d to: %s and pos %d to: %s\n", j, trick[j],my_pos,trick[my_pos]);
                            //answer->data = (Uint8 *) strdup("");
                            for (int n = 0; n < 4; n++) {
                                strcat(answer->data, (Uint8 *) trick[n]);
                                strcat(answer->data, ";");
                            }
                            printf("\npacket: %s\n", answer->data);
                            answer->len = 13;
                            memcpy(&udPpacket, answer, sizeof(UDPpacket));
                            printf("överkopierad data: %s\n", udPpacket.data);
                            if ((len = SDLNet_UDP_Send(sd, channel, answer))); //done = 1;

                        }
                    }
                    else if ((j + 2) % 4 == my_pos && (strcmp(trick[(j + 1) % 4], "FF")) &&!(strcmp(trick[my_pos], "FF"))){
                        strcpy(trick[my_pos], hand[0]);
                        printf("changed trick pos %d to: %s\n", j, trick[j]);
                        //answer->data = (Uint8 *) strdup("");
                        for (int n = 0; n < 4; n++) {
                            strcat(answer->data, (Uint8 *) trick[n]);
                            strcat(answer->data, ";");
                        }
                        printf("\npacket: %s\n", answer->data);
                        answer->len = 13;
                        memcpy(&udPpacket, answer, sizeof(UDPpacket));
                        printf("överkopierad data: %s\n", udPpacket.data);
                        if ((len = SDLNet_UDP_Send(sd, channel, answer)));
                    }
                    else if (!(strcmp(trick[my_pos], "FF")) && (strcmp(trick[(my_pos + 3) % 4], "FF")) && (strcmp(trick[(my_pos + 3) % 4], "EE"))){
                        strcpy(trick[my_pos], hand[0]);
                        printf("changed trick pos %d to: %s\n", j, trick[j]);
                        //answer->data = (Uint8 *) strdup("");
                        for (int n = 0; n < 4; n++) {
                            strcat(answer->data, (Uint8 *) trick[n]);
                            strcat(answer->data, ";");
                        }
                        printf("\npacket: %s\n", answer->data);
                        answer->len = 13;
                        memcpy(&udPpacket, answer, sizeof(UDPpacket));
                        printf("överkopierad data: %s\n", udPpacket.data);
                        if ((len = SDLNet_UDP_Send(sd, channel, answer)));
                    }

                }

                fd = fopen(log_string, "a+");
                printf("Emottaget: %s\n", packet->data);
                printf("skickad data: %s\n", udPpacket.data);

                fclose(fd);
                sleep(2);
                // SDLNet_FreePacket this packet when finished with it
                SDLNet_FreePacket(packet);
            }
        }
        sleep(2);

    }

    return 0;
}

