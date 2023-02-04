CC=gcc
CFLAGS=-Wall -Wextra -Werror

all: editor

editor: editor.c editor.h
	$(CC) $(CFLAGS) -O2 -o editor $<

debug: editor.c editor.h
	$(CC) $(CFLAGS) -g -O0 -o editor $<

clean:
	rm -f editor
