// client.c
// Compile with cc -std=c99 -Wall -lcurl client.c -o client

#include <stdio.h>
#include <stdlib.h>
#include "jsmn.h"
#include <curl/curl.h>

int main( int argc, char** argv ) {
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if( curl ) {
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append( chunk, "Content-Type: application/json" );
		curl_easy_setopt( curl, CURLOPT_URL, "http://colorfight.herokuapp.com/getgameinfo" );
		curl_easy_setopt( curl, CURLOPT_POSTFIELDS, "{\"protocol\": 2, \"display\": true}" );
		curl_easy_setopt( curl, CURLOPT_HTTPHEADER, chunk );
		res = curl_easy_perform( curl );
		if( res != CURLE_OK ) {
			fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror( res ) );
		}
		curl_easy_cleanup( curl );
	}
	return 0;
}