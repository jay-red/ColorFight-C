// main.c
// Compile with cc -std=c99 -Wall -lcurl main.c -o main

#include <stdio.h>
#include <stdlib.h>
#include "client.h"

int main( int argc, char** argv ) {
	//char *responseText = post_json( "http://colorfight.herokuapp.com/getgameinfo", "{\"protocol\": 2, \"display\": true}" );
	refresh();
}