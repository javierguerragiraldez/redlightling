#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>

#include "libs/lmdb.h"

#include "utils_l.h"
#include "api.h"
#include "store.h"

#define chk(w,f)	do { if(!err) {err=_chk(w,f); } } while(0)


typedef struct conn_t {
	rl_db db;
} *conn_t;


conn_t rl_new_conn (const char* path) {
	return (conn_t) rl_open(path);
}


err_t do_set(conn_t conn, const arg_t *args) {
	err_t err = MDB_SUCCESS;
	rl_record *rec = rl_new_record(&conn->db);
	if (! rec) return ENOMEM;
	
	rl_set_key(rec, args[0].data, args[0].size);
	rec->data.mv_data = args[1].data;
	rec->data.mv_size = args[1].size;
	
	chk("do_set begin_write", rl_begin_write(&conn->db));
	chk("do_set rl_write", rl_write(rec));
	chk("do_set end_write", rl_end_write(&conn->db));
	
	if (err) {
		h_txn_abort(&conn->db);
	}
	free(rec);
	return err;
}

err_t do_get(conn_t conn, arg_t *args) {
	err_t err = MDB_SUCCESS;
	rl_record *rec = rl_new_record(&conn->db);
	if (! rec) return ENOMEM;
	
	rl_set_key(rec, args[0].data, args[0].size);
	
	chk("do_get begin_read", rl_begin_read(&conn->db));
	chk("do_get fetch", rl_fetch(rec));
	chk("do_get end_read", rl_end_read(&conn->db));
	
	args[1].data = rec->data.mv_data;
	args[1].size = rec->data.mv_size;
	free(rec);
	return err;
}

err_t do_del(conn_t conn, arg_t *args) {
	err_t err = MDB_SUCCESS;
	rl_record *rec = rl_new_record(&conn->db);
	if (! rec) return ENOMEM;
	
	rl_set_key(rec, args[0].data, args[0].size);
	chk("do_del begin_write", rl_begin_write(&conn->db));
	chk("do_del delete", rl_delete(rec));
	chk("do_del end_write", rl_end_write(&conn->db));
	free(rec);
	return err;
}


err_t rl_do (conn_t conn, cmd_t cmd, int n, arg_t *args) {
	switch (cmd) {
		case c_NOOP:
			return 0;
			
		case c_SET:
			if (n != 2) return MDB_INVALID;
			return do_set(conn, args);
			
		case c_GET:
			if (n != 2) return MDB_INVALID;
			if (args[1].data != NULL || args[1].size != 0) return MDB_INVALID;
			return do_get(conn, args);
			
		case c_DELETE:
			if (n != 1) return MDB_INVALID;
			return do_del(conn, args);
			
		default:
			return MDB_INVALID;
	}
}
