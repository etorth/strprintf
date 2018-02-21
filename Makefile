all:
	gcc -std=c11 -c -g strprintf.c
	ar rvs libstrprintf.a strprintf.o
clean:
	rm -rf *.o *.a
