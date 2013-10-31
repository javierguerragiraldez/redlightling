
_log("open/clean/close")

ffi.cdef [[
	typedef struct MDB_env MDB_env;
	typedef struct MDB_env MDB_txn;
	typedef unsigned int MDB_dbi;
	typedef struct MDB_cursor MDB_cursor;
	typedef struct MDB_val {
		size_t		 mv_size;	/**< size of the data item */
		uint8_t		*mv_data;	/**< address of the data item */
	} MDB_val;

]]
ffi.cdef (assert(io.open('store.h')):read('*a'))

local h = assert(C.rl_open('testdb/'), "can't open DB")
assert (C.rl_clean_db(h), "can't clean db")
C.rl_close(h)

_log("read inexistant")
local k = "key1"
local h = assert(C.rl_open('testdb/'), "can't open DB")
local r = assert(C.rl_new_record(h), "can't alloc record")
-- _showrec(r, "new rec")
C.rl_set_key(r, _buflen(k))
-- _showrec(r, "with key1")


assert(C.rl_begin_read(h))
local d = assert(C.rl_read(r) == nil, "shouldn't find this")
C.rl_end_read(h)

_log("store data")
r.data.mv_data, r.data.mv_size = _buflen("stored data")

assert(C.rl_begin_write(h))
assert(C.rl_write(r))
C.rl_end_write(h)

_log("read stored data")
local r2 = assert(C.rl_new_record(h), "can't alloc record 2")
C.rl_set_key(r2, _buflen(k))

assert(C.rl_begin_read(h))
local d = assert(C.rl_fetch(r2), "can't find record")
C.rl_end_read(h)
