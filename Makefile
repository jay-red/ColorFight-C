all: main
	./main

main: main.c client.c client.h
	cc -std=c99 -Wall -lcurl main.c client.c -o main