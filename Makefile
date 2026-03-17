CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = p

all: $(TARGET)

$(TARGET): main.o ui.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o ui.o

main.o: main.c ui.h
	$(CC) $(CFLAGS) -c main.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

clean:
	rm -f *.o $(TARGET)