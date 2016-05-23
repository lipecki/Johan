all: clean account.o encrypt.o InetS3.o port_hearts.o players.o game.o serve_client.o serve_client game_server.o game_server #gameserver.o gameserver InetS3

clean:
	rm -f *.o
	rm -f game_server
	rm -f InetS3
	rm -f serve_client
CC := gcc -std=c99 -g -w
SDL_FLAGS := `sdl2-config --cflags`

account.o: 
	$(CC) -c `mysql_config --cflags --libs` -lmysql account.c

encrypt.o:
	$(CC) -std=c99 -c -lmcrypt encrypt.c

gameserver.o:
	$(CC) -c `sdl2-config --cflags` `mysql_config --cflags --libs` gameserver.c

players.o:
	$(CC) -c `sdl2-config --cflags` -lSDL2_net players.c

game.o:
	$(CC) -c game.c

gameserver:
	$(CC) -o -pthread game_server game_server.o encrypt.o account.o players.o game.o `sdl2-config --libs` `mysql_config --libs` -lSDL2_net -lmysqlclient -lmcrypt -pthread

port_hearts.o:
	$(CC] -c `mysql_config --cflags --libs` port_hearts.c

InetS3.o:
	gcc -c InetS3.c `mysql_config --cflags --libs` -lmysqlclient

InetS3:
	gcc -o InetS3 port_hearts.o InetS3.o account.o encrypt.o game.o -lmysqlclient -lmcrypt

serve_client.o:
	$(CC) -c `mysql_config --cflags --libs` -lmysqlclient serve_client.c

serve_client:
	$(CC) -o serve_client serve_client.o

game_server.o:
	$(CC) -c `mysql_config --cflags --libs` -lmysqlclient game_server.c

game_server:
	$(CC) -o game_server game_server.o


