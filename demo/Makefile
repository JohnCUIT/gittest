CFLAG=-I./include -L./lib

all:
	gcc -o test test.c $(CFLAG) -lmutexwatch -lpthread -llstLib

clean:
	rm -fr test
