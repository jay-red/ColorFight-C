// main.c
// Compile with cc -std=c99 -Wall -lcurl main.c -o main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "jsmn.h"

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
		//curl_easy_setopt( curl, CURLOPT_POSTFIELDS, data );
		//curl_easy_setopt( curl, CURLOPT_HTTPHEADER, chunk );
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

int refresh() {
	char *responseText = post_json( "https://pastebin.com/raw/uJ80RjqT", "{\"protocol\": 2, \"display\": true}" );
	printf( "%s", responseText );
	free( responseText );
	return 1;
}