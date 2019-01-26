// main.c
// Compile with cc -std=c99 -Wall -lcurl main.c -o main

#include <stdio.h>
#include <stdlib.h>
#include "client.h"

int main( int argc, char** argv ) {
	//char *responseText = post_json( "http://colorfight.herokuapp.com/getgameinfo", "{\"protocol\": 2, \"display\": true}" );
	struct game *g = malloc( sizeof( *g ) );
	struct cell c;
	struct cell *cc;
	init_game( g );
	JoinGame( g, "MyAI" );
	int x;
	int y;
	int d;
	int data;
	int index;
	while( 1 ) {
		Refresh( g );
		for( x = 0; x < 30; x++ ) {
			for( y = 0; y < 30; y++ ) {
				c = g->cells[ y * 30 + x ];
				if( c.owner == g->uid ) {
					d = ( int ) ( ( ( double ) rand() / RAND_MAX ) * 4 );
					if( d == 0 ) {
						cc = GetCell( g, x, y - 1 );
					} else if( d == 1 ) {
						cc = GetCell( g, x + 1, y );
					} else if( d == 2 ) {
						cc = GetCell( g, x, y + 1 );
					} else if( d == 3 ) {
						cc = GetCell( g, x - 1, y );
					}
					if( cc != NULL ) {
						if( cc->owner != g->uid && cc->takeTime <= 4.0 && !cc->isTaking ) {
							data = AttackCell( g, cc->x, cc->y, 0 );
						}
					}
				}
			}
		}
	}
	AttackCell( g, 23, 0, 0 );
	free( g->cells );
	free( g->token );
	free( g );
}