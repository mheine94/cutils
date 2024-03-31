all: tests
	echo "all"

tests: tests.c arraylist.o linkedlist.o stream.o 
	gcc -g arraylist.o linkedlist.o stream.o tests.c -o tests

arraylist.o: arraylist.c
	gcc -g -c arraylist.c -o arraylist.o

linkedlist.o: linkedlist.c
	gcc -g -c linkedlist.c -o linkedlist.o

stream.o: stream.c arraylist.o
	gcc -g -c arraylist.o stream.c -o stream.o

error_or_value: clean
	gcc -g error_or_value.c -o error_or_value

clean:
	rm -f stream error_or_value stream.exe error_or_value.exe
