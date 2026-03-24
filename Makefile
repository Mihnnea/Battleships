CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lraylib -lGL -lm -lpthread -ldl -lX11

TARGET = battleships

all: $(TARGET)

$(TARGET): main.o ui.o player.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o ui.o player.o $(LIBS)

main.o: main.c ui.h player.h
	$(CC) $(CFLAGS) -c main.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

clean:
	rm -f *.o $(TARGET)