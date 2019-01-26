// main.c
// Compile with cc -std=c99 -Wall -lcurl main.c -o main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "client.h"

char buffer[ 64 ];
int i;

void init_game( struct game *g ) {
	g->token = malloc( 64 * sizeof( *( g->token ) ) );
	g->token[ 0 ] = '\0';
	g->cells = malloc( 900 * sizeof( *( g->cells ) ) );
	/*for( int j = 0; j < 900; j++ ) {
		g->cells[ j ] = malloc( sizeof( struct cell ) );
	}*/
}

void init_response( struct response *r ) {
	r->size = 0;
	r->text = malloc( 1 );
	r->text[ 0 ] = '\0';
}

size_t response_handler( void *ptr, size_t size, size_t nmemb, struct response *r ) {
	size_t respSize = size * nmemb;
	r->text = realloc( r->text, r->size + respSize + 1 );
	if( r->text == NULL ) exit( EXIT_FAILURE ); //Exit on realloc failure
	memcpy( r->text + r->size, ptr, respSize );
	r->text[ r->size + respSize ] = '\0';
	r->size += respSize;
	return respSize;
}

char *post_json( const char *url, const char *data ) {
	CURL *curl;
	CURLcode res;
	struct response r ;
	init_response( &r );
	curl = curl_easy_init();
	if( curl ) {
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append( chunk, "Content-Type: application/json" );
		curl_easy_setopt( curl, CURLOPT_URL, url );
		curl_easy_setopt( curl, CURLOPT_POSTFIELDS, data );
		curl_easy_setopt( curl, CURLOPT_HTTPHEADER, chunk );
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, response_handler );
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &r );
		res = curl_easy_perform( curl );
		if( res != CURLE_OK ) {
			fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror( res ) );
		}
		curl_easy_cleanup( curl );
	}
	return (&r)->text;
}

void nextValue( char *responseText ) {
	int foundValue = 0;
	int bufferPosition = 0;
	for( ; responseText[ i ]; i++ ) {
		if( responseText[ i ] == ':' ) {
			if( responseText[ i + 1 ] == '"' ) {
				i++;
			}
			foundValue = 1;
		} else if( responseText[ i ] == ',' || responseText[ i ] == '}' || responseText[ i ] == '"' ) {
			if( responseText[ i ] == '"' ) {
				i++;
			}
			buffer[ bufferPosition ] = '\0';
			return;
		} else if( foundValue ) {
			buffer[ bufferPosition++ ] = responseText[ i ];
		}
	}
}

int nextKey( char *responseText ) {
	int foundString = 0;
	int bufferPosition = 0;
	for( ; responseText[ i ]; i++ ) {
		if( responseText[ i ] == '"' ) {
			foundString = !foundString;
			if( !foundString ) {
				buffer[ bufferPosition ] = '\0';
				return 1;
			}
		} else if( foundString ) {
			buffer[ bufferPosition++ ] = responseText[ i ];
		}
	}
	return 0;
}

int matchKey( char *responseText, char *key ) {
	if( strcmp( buffer, key ) == 0 ) {
		nextValue( responseText );
		return 1;
	}
	return 0;
}

void printCell( struct cell *c ) {
	printf( "Owner: %d\n", c->owner );
	printf( "Attacker: %d\n", c->attacker );
	printf( "Taking: %d\n", c->isTaking );
	printf( "X: %d\n", c->x );
	printf( "Y: %d\n", c->y );
	printf( "Occupy Time: %f\n", c->occupyTime );
	printf( "Attack Time: %f\n", c->attackTime );
	printf( "Take Time: %f\n", c->takeTime );
	printf( "Finish Time: %f\n", c->finishTime );
	printf( "Cell Type: %d\n", c->cellType );
	printf( "Base: %d\n", c->isBase );
	printf( "Building: %d\n", c->isBuilding );
	printf( "Build Time: %f\n\n", c->buildTime );
}

void processCells( struct game *g, char *responseText ) {
	int cellIndex = 0;
	for( ; responseText[ i ] != '['; i++ ) {

	}
	while( responseText[ i + 1 ] != ']' ) {
		nextKey( responseText );
		i++;
		if( matchKey( responseText, "o" ) ) {
			g->cells[ cellIndex ].owner = atoi( buffer );
		} else if( matchKey( responseText, "a" ) ) {
			g->cells[ cellIndex ].attacker = atoi( buffer );
		} else if( matchKey( responseText, "c" ) ) {
			g->cells[ cellIndex ].isTaking = atoi( buffer ) == 1;
		} else if( matchKey( responseText, "x" ) ) {
			g->cells[ cellIndex ].x = atoi( buffer );
		} else if( matchKey( responseText, "y" ) ) {
			g->cells[ cellIndex ].y = atoi( buffer );
		} else if( matchKey( responseText, "ot" ) ) {
			g->cells[ cellIndex ].occupyTime = atof( buffer );
		} else if( matchKey( responseText, "at" ) ) {
			g->cells[ cellIndex ].attackTime = atof( buffer );
		} else if( matchKey( responseText, "t" ) ) {
			g->cells[ cellIndex ].takeTime = atof( buffer );
		} else if( matchKey( responseText, "f" ) ) {
			g->cells[ cellIndex ].finishTime = atof( buffer );
		} else if( matchKey( responseText, "ct" ) ) {
			if( strcmp( buffer, "energy" ) == 0 ) {
				g->cells[ cellIndex ].cellType = 1;
			} else if( strcmp( buffer, "gold" ) == 0 ) {
				g->cells[ cellIndex ].cellType = 2;
			} else {
				g->cells[ cellIndex ].cellType = 0;
			}
		} else if( matchKey( responseText, "b" ) ) {
			g->cells[ cellIndex ].isBase = strcmp( buffer, "base" ) == 1;
		} else if( matchKey( responseText, "bf" ) ) {
			g->cells[ cellIndex ].isBuilding = strcmp( buffer, "false" ) == 0	;
		} else if( matchKey( responseText, "bt" ) ) {
			g->cells[ cellIndex ].buildTime = atof( buffer );
		} else {
			nextValue( responseText );
		}
		if( responseText[ i ] == '}' ) {
			//printf( "Cell %d\n", cellIndex );
			//printCell( g->cells + cellIndex );
			cellIndex += 1;
		}
	}
}

void processUsers( struct game *g, char *responseText ) {
	for( ; responseText[ i ] != '['; i++ ) {
		
	}
	while( responseText[ i + 1 ] != ']' ) {
		nextKey( responseText );
		printf( "Key: %s\n", buffer );
		i++;
		nextValue( responseText );
		printf( "Value: %s\n\n", buffer );
	}
}

void processInfo( struct game *g, char *responseText ) {
	for( ; responseText[ i ] != '{'; i++ ) {

	}
}

int Refresh( struct game *g ) {
	char *responseText = post_json( "http://colorfight.herokuapp.com/getgameinfo", "{\"protocol\": 2}" );
	i = 0;
	buffer[ 0 ] = '\0';
	int nextExists = nextKey( responseText );
	while( nextExists ) {
		printf( "%s\n", buffer );
		if( strcmp( buffer, "users" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Users" );
			processUsers( g, responseText );
		} else if( strcmp( buffer, "cells" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Cells" );
			processCells( g, responseText );
		} else if( strcmp( buffer, "info" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Info" );
			processInfo( g, responseText );	
		}
		nextExists = nextKey( responseText );
	}
	printf( "%c", '\n' );
	free( responseText );
	return 1;
}

int CheckToken( struct game *g, char *name, char *token ) {
	printf( "Checking token: %s\n", token );
	printf( "%s\n", "break1" );
	char *data = malloc( 64 * sizeof( *data ) );
	printf( "%s\n", "break2" );
	char *tempName = malloc( 64 * sizeof( *tempName ) );
	printf( "%s\n", "break3" );
	data[ 0 ] = '\0';
	printf( "%s\n", "break4" );
	strcat( data, "{\"token\":\"" );
	printf( "%s\n", "break5" );
	strcat( data, token );
	printf( "%s\n", "break6" );
	strcat( data, "\"}" );
	printf( "%s\n", "break7" );
	char *responseText = post_json( "http://colorfight.herokuapp.com/checktoken", data );
	printf( "%s\n", "break8" );
	free( data );
	printf( "%s\n", "break9" );
	i = 0;
	int nextExists = nextKey( responseText );
	while( nextExists ) {
		i++;
		printf( "%s\n", buffer );
		if( matchKey( responseText, "uid" ) ) {
			printf( "%s\n", buffer );
			g->uid = atoi( buffer );
		} else if( matchKey( responseText, "name" ) ) {
			strcat( tempName, buffer );
			if( strcmp( tempName, name ) != 0 ) {
				free( responseText );
				free( tempName );
				return 0;
			}
			free( tempName );
		} else if( matchKey( responseText, "msg" ) ) {
			free( responseText );
			return 0;
		} else {
			nextValue( responseText );
		}
		nextExists = nextKey( responseText );
	}
	free( responseText );
	strcat( g->token, token );
	return 1;
}

int JoinGame( struct game *g, char *name ) {
	printf( "Joining game as: %s\n", name );
	FILE *tokenPtr;
	tokenPtr = fopen( "token", "r" );
	if( tokenPtr != NULL ) {
		char *token = malloc( 64 * sizeof( *token ) );;
		fscanf( tokenPtr, "%s", token );
		fclose( tokenPtr );
		if( CheckToken( g, name, token ) ) {
			return 1;
		}
	}
	char *responseText;
	char *data = malloc( 64 * sizeof( *data ) );
	data[ 0 ] = '\0';
	strcat( data, "{\"name\":\"" );
	strcat( data, name );
	strcat( data, "\"}" );
	responseText = post_json( "http://colorfight.herokuapp.com/joingame", data );
	free( data );
	printf( "%s\n", responseText );
	i = 0;
	int nextExists = nextKey( responseText );
	while( nextExists ) {
		i++;
		printf( "%s\n", buffer );
		if( matchKey( responseText, "uid" ) ) {
			printf( "%s\n", buffer );
			g->uid = atoi( buffer );
		} else if( matchKey( responseText, "token" ) ) {
			strcat( g->token, buffer );
			printf( "%s\n", g->token );
			tokenPtr = fopen( "token", "w" );
			fprintf( tokenPtr, "%s", g->token );
			fclose( tokenPtr );
		} else {
			nextValue( responseText );
		}
		nextExists = nextKey( responseText );
	}
	free( responseText );
	return 1;
}

int AttackCell( struct game *g, int x, int y, int boost ) {
	char *data = malloc( 100 * sizeof( *data ) );
	char *cellx = malloc( 2 * sizeof( *cellx ) );
	char *celly = malloc( 2 * sizeof( *celly ) );
	sprintf( cellx, "%d", x );
	sprintf( celly, "%d", y );
	data[ 0 ] = '\0';
	strcat( data, "{\"cellx\":" );
	strcat( data, cellx );
	strcat( data, ",\"celly\":" );
	strcat( data, celly );
	strcat( data, ",\"boost\":" );
	if( boost ) {
		strcat( data, "true" );
	} else {
		strcat( data, "false" );
	}
	strcat( data, ",\"token\":\"" );
	strcat( data, g->token );
	strcat( data, "\"}" );
	char* responseText = post_json( "http://colorfight.herokuapp.com/attack", data );
	free( data );
	printf( "%s\n", responseText );
	i = 0;
	int nextExists = nextKey( responseText );
	while( nextExists ) {
		i++;
		if( matchKey( responseText, "err_code" ) ) {
			return atoi( buffer );
		} else {
			nextValue( responseText );
		}
		nextExists = nextKey( responseText );
	}
	free( responseText );
	return 1;
}

struct cell *GetCell( struct game *g, int x, int y ) {
	if( x < 0 || x > 29 || y < 0 || y > 29 ) {
		return NULL;
	}
	return g->cells + ( y * 30 + x );
}

/*for( i = 0; responseText[ i ]; i++ ) {
		if( responseText[ i ] == '{' ) {
			++braces;
		} else if( responseText[ i ] == '}' ) {
			--braces;
		} else if( responseText[ i ] == '\\' ) {
			escaped = 1;
		} else if( !escaped && responseText[ i ] == '"' ) {
			foundString = !foundString;
			if( !foundString ) {
				if( responseText[ i + 1 ] == ':' ) {
					printf( "%s", "Key Found: " );
				}
				buffer[ bufferIndex ] = '\0';
				bufferIndex = 0;
				printf( "%s", buffer );
				printf( "%c", '\n' );
			}
		} else if( foundString) {
			//printf( "%c", responseText[ i ] );
			buffer[ bufferIndex++ ] = responseText[ i ];
		}
		if( escaped ) {
			escaped = 0;
		}
		if( responseText[ i + 1 ] == '}' && braces - 1 == 0 ) {
			printf( "%c", '\n' );
			break;
		}
	}*/	