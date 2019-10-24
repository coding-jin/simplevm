
all : test
test: ../my_vm.h
	gcc test2.c -L../ -lmy_vm -o test2 -m64
	#gcc test1.c -L../ -lmy_vm -m32 -o test1
	#gcc test.c -L../ -lmy_vm -m32 -o test
	#gcc multi_test.c -L../ -lmy_vm -m32 -pthread -o mtest
clean:
	rm -rf test2
