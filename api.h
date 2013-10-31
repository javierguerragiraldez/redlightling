
typedef struct {
	size_t size;
	uint8_t *data;
} arg_t;

typedef int err_t;
enum {
	noErr = 0,
};

typedef enum {
	c_NOOP = 0,
	c_SET,
	c_GET,
	c_DELETE,
} cmd_t;

typedef struct conn_t *conn_t;

conn_t rl_new_conn(const char *path);
err_t rl_do(conn_t conn, cmd_t cmd, int n, arg_t* args);