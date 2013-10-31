
#include <stdint.h>
#include <stddef.h>

#include "libs/lmdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils_l.h"
#include "store.h"

#define Z(sp)	memset(sp,0,sizeof(*sp));

#define chk(w,f)	do { if(!err) {err=_chk(w,f); } } while(0)


int _chk(const char *where, int err) {
	if (err) {
		char s[4096];
		sprintf(s, "(%s, %d): %s", where, err, mdb_strerror(err));
		perror(s);
	}
	return err;
}


void h_dbi_close(rl_db *h) {
	if (h->dbi) mdb_dbi_close(h->env, h->dbi);
	h->dbi = 0;
}

void h_txn_abort(rl_db *h) {
	if (h->txn) mdb_txn_abort(h->txn);
	h->txn = NULL;
}

void h_env_close(rl_db *h) {
	if (h->env) mdb_env_close(h->env);
	h->env = NULL;
}

rl_db *rl_kill(rl_db *h) {
	if (h) {
		h_dbi_close(h);
		h_env_close(h);
		free(h);
	}
	return NULL;
}

/**
 * opens / creates a database
 */
rl_db *rl_open (const char* path) {
	int err = MDB_SUCCESS;
	rl_db *h = malloc(sizeof(rl_db));
	if (!h) return NULL;
	Z(h);
	chk("rl_open env_create", mdb_env_create(&h->env));
	chk("rl_open env_open", mdb_env_open(h->env, path, 0, 0660));
	
	if (err) return rl_kill(h);
	
	return h;
}

void rl_close (rl_db* h) {
	if (h->env && h->dbi) {
		if (h->txn) {
			mdb_txn_abort(h->txn);
			h->txn = NULL;
		}
		mdb_dbi_close(h->env, h->dbi);
		h->dbi = 0;
		h->env = NULL;
	}
}



/**
 * begins a transaction
 */
static int begin_txn (rl_db* h, unsigned int flags) {
	int err = MDB_SUCCESS;
	if (!h->env) return MDB_INVALID;
	if (h->txn && flags & MDB_RDONLY) {
		chk("begin_txn renew", mdb_txn_renew((h->txn)));
	} else {
		if (h->txn) mdb_txn_abort(h->txn);
		chk("begin_txn txn_begin", mdb_txn_begin(h->env, NULL, flags, &h->txn));
	}
	chk("begin_txn dbi_open", mdb_dbi_open(h->txn, NULL, 0, &h->dbi));
	
	if (err) {
		h_dbi_close(h);
		h_txn_abort(h);
	}
	return err;
}


int rl_begin_read (rl_db* h) {
	return begin_txn(h, MDB_RDONLY);
}

int rl_begin_write (rl_db* h) {
	return begin_txn(h, 0);
}


int rl_end_read ( rl_db* h ) {
	mdb_txn_reset(h->txn);
	return MDB_SUCCESS;
}

int rl_end_write (rl_db* h) {
	int err = MDB_SUCCESS;
	chk("end_write txn_commit", mdb_txn_commit(h->txn));
	if (!err)
		h->txn = NULL;
	return err;
}


int rl_clean_db (rl_db* h) {
	int err = MDB_SUCCESS;
	chk("clean_db begin_write", rl_begin_write(h));
	chk("clean_db drop", mdb_drop(h->txn, h->dbi, 0));
	chk("clean_db end_write", rl_end_write(h));
	return err;
}



rl_record *rl_new_record (rl_db* h) {
	rl_record *r = malloc(sizeof(rl_record));
	if (!r) return NULL;
	Z(r);
	r->db = h;
	
	return r;
}


void rl_set_key (rl_record *rec, uint8_t *key, size_t keysize) {
	if (!rec) return;
	rec->key.mv_data = key;
	rec->key.mv_size = keysize;
}


static int fixKey(rl_record *rec) {
	return MDB_SUCCESS;
}


static int get_cursor(rl_record *rec) {
	if (rec->cursor)
		return MDB_SUCCESS;
	return _chk("get_cursor cursor_open", mdb_cursor_open(rec->db->txn, rec->db->dbi, &rec->cursor));
}

int rl_fetch(rl_record *rec) {
	int err = MDB_SUCCESS;
	chk("fetch fixKey", fixKey(rec));
	chk("fetch get_cursor", get_cursor(rec));
	chk("fetch cursor_get", mdb_cursor_get(rec->cursor, &rec->key, &rec->data, MDB_SET_RANGE));
	return err;
}

uint8_t *rl_read(rl_record *rec) {
	int err = rl_fetch(rec);
	if (err == MDB_NOTFOUND || _chk("rl_fetch ||", err))
		return NULL;
	
	return rec->data.mv_data;
}
// uint8_t *rl_fetch (rl_record *rec) {
// 	fixKey(rec);
// 	MDB_cursor *c = get_cursor(rec);
// 	if (!c) return NULL;
// 	
// 	int r = mdb_cursor_get(c, &rec->key, &rec->data, MDB_SET_RANGE);
// 	if (r == MDB_NOTFOUND || _chk("rl_fetch ||", r))
// 		return NULL;
// 	
// 	return rec->data.mv_data;
// }

int rl_write (rl_record* rec) {
	return _chk("rl_write put", mdb_put(rec->db->txn, rec->db->dbi, &rec->key, &rec->data, 0));
}

int rl_delete (rl_record *rec) {
	int err = rl_fetch(rec);
	chk("delete cursor_del", mdb_cursor_del(rec->cursor, 0));
	return err;
}