

EXEC=rlb
SRCS=rlb.c api.c plank.c store.c siphash.c utils_l.c
HDRS=api.h plank.h store.h utils_l.h
# CFLAGS = -O3 -Wall
CFLAGS = -g -Wall -I libs/
LDLIBS = -L libs/ -lluajit-5.1 -llmdb -lm -lpthread
LDFLAGS = -Wl,-E


OBJS=$(subst .c,.o,$(SRCS))


all: $(EXEC)

%.o: %.c $(HDRS)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(EXEC): $(OBJS) $(HDRS)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LDLIBS) 

# tool.o: tool.c support.h

# support.o: support.h support.c

clean:
	$(RM) $(OBJS)
	$(RM) $(EXEC)

test: $(EXEC)
	./$(EXEC) -x test.lua