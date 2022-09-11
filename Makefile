CC = gcc -g

CFLAGS= -D_GNU_SOURCE

LIBS= 

.PHONY:	libprocesshide.so	clean

libcdromguard.so: processhide.c
	$(CC) processhide.c -fPIC -shared $(CFLAGS) $(LIBS) -ldl -o ../bin/libprocesshide.so

clean:
	-rm ../bin/libprocesshide.so

