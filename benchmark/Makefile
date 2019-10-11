
all : test
test: ../my_vm.h
	gcc test3.c -L../ -lmy_vm -m32 -o test3
	#gcc test2.c -L../ -lmy_vm -m32 -o test2
	#gcc test1.c -L../ -lmy_vm -m32 -o test1
	#gcc test0.c -L../ -lmy_vm -m32 -o test0
	#gcc test.c -L../ -lmy_vm -m32 -o test
	#gcc multi_test.c -L../ -lmy_vm -m32 -pthread -o mtest
clean:
	rm -rf test3
