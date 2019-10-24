CC = gcc
CFLAGS = -g -c -std=c11 -m64
AR = ar -rc
RANLIB = ranlib

all: my_vm.a

my_vm.a: my_vm.o
	$(AR) libmy_vm.a my_vm.o
	$(RANLIB) libmy_vm.a

my_vm.o: my_vm.h

	$(CC)	$(CFLAGS)  my_vm.c

clean:
	rm -rf *.o *.a
