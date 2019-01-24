#include <string.h>

struct response;
struct cell;
struct user;
void init_response( struct response *r );
size_t response_handler( void *ptr, size_t size, size_t nmemb, struct response *r );
char *post_json( const char* url, const char *data );
int refresh();