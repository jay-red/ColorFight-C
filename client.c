// main.c
// Compile with cc -std=c99 -Wall -lcurl main.c -o main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

char buffer[ 64 ];
int i;

struct user {
	int id;
	char *name;
	double cdTime;
	double buildCdTime;
	int cellNum;
	int baseNum;
	int goldCellNum;
	int energyCellNum;
	int energy;
	int gold;
};

struct cell {
	int owner;
	int attacker;
	int isTaking;
	int x;
	int y;
	float occupyTime;
	float attackTime;
	float takeTime;
	float finishTime;
	int cellType;
	//int buildType;
	int isBase;
	int isBuilding;
	float buildTime;
};

struct response {
	char *text;
	size_t size;
};

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

void processCells( char *responseText ) {
	struct cell *cells = malloc( 900 * sizeof( *cells ) );
	int cellIndex = 0;
	for( ; responseText[ i ] != '['; i++ ) {

	}
	while( responseText[ i + 1 ] != ']' ) {
		nextKey( responseText );
		i++;
		if( matchKey( responseText, "o" ) ) {
			cells[ cellIndex ].owner = atoi( buffer );
		} else if( matchKey( responseText, "a" ) ) {
			cells[ cellIndex ].attacker = atoi( buffer );
		} else if( matchKey( responseText, "c" ) ) {
			cells[ cellIndex ].isTaking = atoi( buffer ) == 1;
		} else if( matchKey( responseText, "x" ) ) {
			cells[ cellIndex ].x = atoi( buffer );
		} else if( matchKey( responseText, "y" ) ) {
			cells[ cellIndex ].y = atoi( buffer );
		} else if( matchKey( responseText, "ot" ) ) {
			cells[ cellIndex ].occupyTime = atof( buffer );
		} else if( matchKey( responseText, "at" ) ) {
			cells[ cellIndex ].attackTime = atof( buffer );
		} else if( matchKey( responseText, "t" ) ) {
			cells[ cellIndex ].takeTime = atof( buffer );
		} else if( matchKey( responseText, "f" ) ) {
			cells[ cellIndex ].finishTime = atof( buffer );
		} else if( matchKey( responseText, "ct" ) ) {
			if( strcmp( buffer, "energy" ) == 0 ) {
				cells[ cellIndex ].cellType = 1;
			} else if( strcmp( buffer, "gold" ) == 0 ) {
				cells[ cellIndex ].cellType = 2;
			} else {
				cells[ cellIndex ].cellType = 0;
			}
		} else if( matchKey( responseText, "b" ) ) {
			cells[ cellIndex ].isBase = strcmp( buffer, "base" ) == 1;
		} else if( matchKey( responseText, "bf" ) ) {
			cells[ cellIndex ].isBuilding = strcmp( buffer, "false" ) == 0	;
		} else if( matchKey( responseText, "bt" ) ) {
			cells[ cellIndex ].buildTime = atof( buffer );
		} else {
			nextValue( responseText );
		}
		if( responseText[ i ] == '}' ) {
			printf( "Cell %d\n", cellIndex );
			printCell( cells + cellIndex );
			cellIndex += 1;
		}
	}
	free( cells );
}

void processUsers( char *responseText ) {
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

void processInfo( char *responseText ) {
	for( ; responseText[ i ] != '{'; i++ ) {

	}
}

int refresh() {
	char *responseText = post_json( "http://colorfight.herokuapp.com/getgameinfo", "{\"protocol\": 2, \"display\": true}" );
	i = 0;
	buffer[ 0 ] = '\0';
	int nextExists = nextKey( responseText );
	while( nextExists ) {
		printf( "%s\n", buffer );
		if( strcmp( buffer, "users" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Users" );
			processUsers( responseText );
		} else if( strcmp( buffer, "cells" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Cells" );
			processCells( responseText );
		} else if( strcmp( buffer, "info" ) == 0 ) {
			printf( "\n%s\n\n", "Processing Info" );
			processInfo( responseText );	
		}
		nextExists = nextKey( responseText );
	}
	printf( "%c", '\n' );
	free( responseText );
	return 1;
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