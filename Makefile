CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = battleships

all: $(TARGET)

$(TARGET): main.o ui.o player.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o ui.o player.o

main.o: main.c ui.h player.h
	$(CC) $(CFLAGS) -c main.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

clean:
	rm -f *.o $(TARGET)