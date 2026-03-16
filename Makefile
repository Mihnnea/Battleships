CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = p

all: $(TARGET)

$(TARGET): main.o cplx.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o cplx.o

main.o: main.c cplx.h
	$(CC) $(CFLAGS) -c main.c

cplx.o: cplx.c cplx.h
	$(CC) $(CFLAGS) -c cplx.c

clean:
	rm -f *.o $(TARGET)