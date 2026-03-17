CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = p

all: $(TARGET)

# 1. Am adăugat player.o la lista de fișiere necesare pentru executabil
$(TARGET): main.o ui.o player.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o ui.o player.o

# 2. Am adăugat player.h aici (presupunând că incluzi player.h în main.c)
main.o: main.c ui.h player.h
	$(CC) $(CFLAGS) -c main.c

ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c

# 3. Regula nouă pentru compilarea fișierului player
player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

clean:
	rm -f *.o $(TARGET)