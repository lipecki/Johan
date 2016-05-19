all: clean account.o gameserver.o encrypt.o InetS3.o port_hearts.o players.o game.o gameserver InetS3

clean:
	rm -f *.o
	rm -f gameserver
	rm -f InetS3

CC := gcc -std=c99 -g -w
SDL_FLAGS := `sdl2-config --cflags`

account.o: 
	gcc -c `mysql_config --cflags --libs` -lmysql account.c

encrypt.o:
	gcc -std=c99 -c -lmcrypt encrypt.c

gameserver.o:
	gcc -c `sdl2-config --cflags` `mysql_config --cflags --libs` gameserver.c

players.o:
	gcc -c `sdl2-config --cflags` -lSDL2_net players.c

game.o:
	$(CC) -c game.c

gameserver:
	$(CC) -o gameserver gameserver.o encrypt.o account.o players.o game.o `sdl2-config --libs` `mysql_config --libs` -lSDL2_net -lmysqlclient -lmcrypt -lpthread

port_hearts.o:
	gcc -c `mysql_config --cflags --libs` port_hearts.c

InetS3.o:
	gcc -c InetS3.c `mysql_config --cflags --libs` -lmysqlclient

InetS3:
	$(CC) -o InetS3 port_hearts.o InetS3.o account.o encrypt.o game.o -lmysqlclient -lmcrypt

serve_client.o:
        $(CC) $(SDL_FLAGS) serve_client.c

serve_client:
        gcc -o serve_client.o port_hearts.o players.o -lmysqlclient


