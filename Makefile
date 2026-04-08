CC     = gcc
CFLAGS = -Wall -Wextra -O2 -Isrc
LIBS   = -lpthread

all: wordcount

wordcount: src/engine.c src/threadpool.c jobs/wordcount.c
	$(CC) $(CFLAGS) -o wordcount src/engine.c src/threadpool.c jobs/wordcount.c $(LIBS)

clean:
	rm -f wordcount wordcount.exe