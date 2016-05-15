all: gameserver InetS3
clean:
	rm -f *.o gameserver InetS3
.PHONY: all clean

# -c is implicit, you don't need it (it *shouldn't* be there)
# CC is also implicit, you don't need it

CFLAGS := -std=c99 -g -Wall -Wextra

account.o: account.c
	# `mysql_config --cflags --libs` -lmysql

encrypt.o: encrypt.c
	# -lmcrypt

gameserver.o: gameserver.c
	# `sdl2-config --cflags` `mysql_config --cflags --libs` gameserver.c

players.o: players.c
	# `sdl2-config --cflags`

game.o: game.c

gameserver: gameserver.o encrypt.o account.o players.o game.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)                           # `sdl2-config --libs` `mysql_config --libs` -lSDL2_net -lmysqlclient -lmcrypt -lpthread

port_hearts.o: port_hearts.c
	 #`mysql_config --cflags --libs`

InetS3.o: InetS3.c
	    # `mysql_config --cflags --libs` -lmysqlclient

InetS3: InetS3.o account.o encrypt.o port_hearts.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)  # -lmysqlclient

