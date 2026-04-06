CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lraylib -lGL -lm -lpthread -ldl -lX11

TARGET = battleships

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c $(if $(wildcard %.h),%.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)