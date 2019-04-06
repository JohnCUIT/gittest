all:
	gcc -c mutexwatch.c -I../include
	ar rcs libmutexwatch.a *.o
	mv libmutexwatch.a ../lib

clean:
	rm ../lib/libmutexwatch.a *.o
