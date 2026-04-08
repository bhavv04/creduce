CC     = gcc
CFLAGS = -Wall -Wextra -O2 -Isrc

all: wordcount

wordcount: src/engine.c jobs/wordcount.c
	$(CC) $(CFLAGS) -o wordcount src/engine.c jobs/wordcount.c

clean:
	rm -f wordcount