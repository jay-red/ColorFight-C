#include <string.h>

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

struct game {
	char *token;
	//char *name;
	int uid;
	//float endTime;
	//float joinEndTime;
	//int gameId;
	//struct user *users;
	int cellNum;
	int baseNum;
	int goldCellNum;
	int energyCellNum;
	float cdTime;
	float buildCdTime;
	float energy;
	float gold;
	float currTime;
	float lastUpdate;
	struct cell *cells;
};

struct response {
	char *text;
	size_t size;
};

void init_game( struct game *g );
void init_response( struct response *r );
size_t response_handler( void *ptr, size_t size, size_t nmemb, struct response *r );
char *post_json( const char* url, const char *data );
int nextKey( char *responseText );
void nextValue( char *responseText );
int matchKey( char *responseText, char *key );
void printCell( struct cell *c );
void processCells( struct game *g, char *responseText );
void processUsers( struct game *g, char *responseText );
void processInfo( struct game *g, char *responseText );
int Refresh( struct game *g );
int CheckToken( struct game *g, char *name, char *token );
int JoinGame( struct game *g, char *name );
int AttackCell( struct game *g, int x, int y, int boost );
struct cell *GetCell( struct game *g, int x, int y );