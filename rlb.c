
#include <luajit-2.0/luajit.h>
#include <luajit-2.0/lualib.h>
#include <luajit-2.0/lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils_l.h"

// lifted from luajit.c
static int report(lua_State *L, int status)
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		printf ("%s\n", msg);
		lua_pop(L, 1);
	}
	return status;
}


int main (int argc, char *argv[]) {
	int opt;
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	while ((opt = getopt(argc, argv, "e:x:")) != -1) {
		switch (opt) {
			case 'e':
				printf ("exec: %s\n", optarg);
				if (report(L, luaL_dostring(L, optarg))) {
					exit(1);
				}
				break;
			case 'x':
				printf ("debug: %s\n", optarg);
				if (report(L, luaL_dofile(L, optarg))) {
					exit(1);
				}
				break;
		}
	}
	return 0;
}

