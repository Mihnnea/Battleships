# 1. Variabile
CC = gcc
CFLAGS = -Wall -Wextra -g

# Numele executabilului
TARGET = p

# 2. Regula implicită
all: $(TARGET)

# 3. Crearea executabilului (Etapa de Link-are)
$(TARGET): main.o cplx.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o cplx.o

# 4. Compilarea fișierului main.c
main.o: main.c cplx.h
	$(CC) $(CFLAGS) -c main.c

# 5. Compilarea fișierului cplx.c
cplx.o: cplx.c cplx.h
	$(CC) $(CFLAGS) -c cplx.c

# 6. Curățarea fișierelor generate
clean:
	rm -f *.o $(TARGET)