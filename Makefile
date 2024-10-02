# Makefile for compiling Rapid Unix Shell

CC = gcc
CFLAGS = -Wall
SRC = rush.c
OUT = rush

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
