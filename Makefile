all: phunt.c
	gcc -o phunt phunt.c
	gcc -o test test.c

clean:
	rm phunt
	rm test
