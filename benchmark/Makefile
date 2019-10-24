
all : test
test: ../my_vm.h
	#gcc test2.c -L../ -lmy_vm -o test2 -m64 -pthread
	#gcc test1.c -L../ -lmy_vm -m32 -o test1
	#gcc test.c -L../ -lmy_vm -m32 -o test
	gcc multi_test.c -L../ -lmy_vm -m64 -pthread -o mtest -std=c11
clean:
	rm -rf test2; rm -rf mtest
