CC=gcc
CFLAGS=-Wall -Wextra -Werror
OBJS = editor.o common.o console.o

all: editor

editor: clean_objs $(OBJS)
	$(CC) $(CFLAGS) -o editor $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean_objs:
	rm -f $(OBJS)

clean:
	rm -f editor $(OBJS)

debug: clean_objs $(OBJS)
	$(CC) $(CFLAGS) -g -O0 -o editor $(OBJS)
