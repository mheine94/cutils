all:
	gcc -g arraylist.c linkedlist.c stream.c -o stream

error_or_value: clean
	gcc -g error_or_value.c -o error_or_value

clean:
	rm -f stream error_or_value stream.exe error_or_value.exe