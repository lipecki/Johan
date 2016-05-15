all: clean account.o gameserver.o encrypt.o InetS3.o port_hearts.o players.o game.o gameserver InetS3 

clean:
	rm -f *.o
	rm -f gameserver
	rm -f inets3

CFLAGS := -std=c99 -g -Wall

account.o: 
	gcc -c `mysql_config --cflags --libs` -lmysql account.c

encrypt.o:
	gcc -std=c99 -c -lmcrypt encrypt.c

gameserver.o:
	gcc -c `sdl2-config --cflags` `mysql_config --cflags --libs` gameserver.c

players.o:
	gcc -c `sdl2-config --cflags` -lSDL2_net players.c

game.o:
	gcc -std=c99 -c game.c

gameserver:
	$(CC) -o gameserver gameserver.o encrypt.o account.o players.o game.o `sdl2-config --libs` `mysql_config --libs` -lSDL2_net -lmysqlclient -lmcrypt -lpthread

port_hearts.o:
	gcc -c `mysql_config --cflags --libs` port_hearts.c

InetS3.o:
	gcc -c InetS3.c `mysql_config --cflags --libs` -lmysqlclient

InetS3:
	$(CC) -o InetS3 InetS3.o account.o encrypt.o port_hearts.o game.o -lmysqlclient -lmcrypt

