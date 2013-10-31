//store.h

typedef unsigned char Bool;

int _chk(const char *where, int err);

typedef uint64_t hashKey_t[2];

typedef struct rl_db {
	MDB_env *env;
	MDB_txn *txn;
	MDB_dbi dbi;
	hashKey_t hashkey;
} rl_db;

typedef enum rl_rectype {
	rl_null = 0,
	rl_notfound,
	rl_string,
} rl_rectype;

typedef struct rl_stubhash {
	uint64_t hash;
	unsigned char stubb[240-sizeof(uint64_t)];
} rl_stubhash;

typedef struct rl_inn_key {
	uint8_t section;
	uint8_t filler[3];
	rl_stubhash parent;
	rl_stubhash leaf;
} rl_inn_key;

typedef struct rl_record {
	rl_db const *db;
	MDB_cursor *cursor;
	MDB_val key;
	MDB_val data;
	rl_rectype type;
	rl_inn_key inn_key;
} rl_record;

void h_dbi_close(rl_db *h);
void h_txn_abort(rl_db *h);
void h_env_close(rl_db *h);
rl_db *rl_kill(rl_db *h);

extern rl_db *rl_open(const char* path);
extern void rl_close(rl_db *h);
int rl_begin_read(rl_db* h);
int rl_end_read(rl_db* h);
int rl_begin_write(rl_db* h);
extern int rl_end_write(rl_db *h);
extern int rl_clean_db(rl_db *h);
extern rl_record *rl_new_record (rl_db *h);
extern void rl_set_key(rl_record *rec, uint8_t *key, size_t keysize);
extern int rl_fetch(rl_record *rec);
extern uint8_t *rl_read(rl_record *rec);
extern int rl_write(rl_record *rec);
extern int rl_delete (rl_record *rec);

extern void rl_random_hash_key(hashKey_t k);
