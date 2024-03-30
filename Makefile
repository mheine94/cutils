all:
	gcc -g arraylist.c linkedlist.c stream.c -o stream

clean:
	rm *.exe