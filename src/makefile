CC=gcc
CFLAGS=-I.
TARGET=main
CCLIST=query.c corpus.c types.c arraylist/arraylist.c hashmap/hashmap.c trie/trie.c search.c postings/postings.c dictionary/dictionary.c hashtable/hashtable.c

compileLong:
	$(CC) -c -o functions.o functions.c
	$(CC) -c -o trie.o trie/trie.c
	$(CC) -c -o hashmap.o hashmap/hashmap.c
	$(CC) -c -o search.o search.c
	$(CC) -o -lm $(TARGET) functions.o trie.o search.o

compile:
	$(CC) $(CCLIST) -o $(TARGET) -lm

run:
	./$(TARGET)

all: compile run

debug:
	$(CC) -g $(CCLIST) -o $(TARGET) -lm
	gdb ./$(TARGET)
	
test:
	$(CC) hashmap/hashmap.c test.c -o testing
	./testing

testdebug:
	$(CC) -g hashmap/hashmap.c test.c -o testing
	gdb ./testing
