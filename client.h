#include <string.h>

struct response;
struct cell;
struct user;
void init_response( struct response *r );
size_t response_handler( void *ptr, size_t size, size_t nmemb, struct response *r );
char *post_json( const char* url, const char *data );
void nextKey( char *responseText );
void nextValue( char *responseText );
int matchKey( char *responseText, char *key );
void printCell( struct cell *c );
void processCells( char *responseText );
void processUsers( char *responseText );
void processInfo( char *responseText );
int refresh();