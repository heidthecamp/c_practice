all: lookup test

lookup: lookup.c dictionary.c dictionary.h
	gcc -O3 -o lookup $^

test: test.c dictionary.c dictionary.h
	gcc -O3 -o test $^