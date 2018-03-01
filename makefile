
CC = gcc
CFLAGS  = -g -Wall

default: project

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#



project:  main.o functions.o stack.o static_functions.o bloomfilter.o murmur3.o top.o deletion.o schedule.o 
	$(CC) $(CFLAGS) -o project main.o functions.o stack.o static_functions.o bloomfilter.o murmur3.o top.o deletion.o schedule.o  -lm -lpthread

test_project:  test_main.o functions.o stack.o static_functions.o test.o bloomfilter.o murmur3.o top.o deletion.o schedule.o 
	$(CC) $(CFLAGS) -o test_project test_main.o functions.o stack.o static_functions.o test.o bloomfilter.o murmur3.o top.o deletion.o schedule.o  -lm -lpthread


# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#

main.o:  main.c functions.h
	$(CC) $(CFLAGS) -c main.c

test_main.o:  test_main.c functions.h
	$(CC) $(CFLAGS) -c test_main.c



test.o: test.c functions.h
	$(CC) $(CFLAGS) -c test.c

static_functions.o:  static_functions.c static_functions.h 
	$(CC) $(CFLAGS) -c static_functions.c

functions.o:  functions.c functions.h 
	$(CC) $(CFLAGS) -c functions.c

bloomfilter.o:  bloomfilter.c bloomfilter.h 
	$(CC) $(CFLAGS) -c bloomfilter.c

stack.o:  stack.c stack.h 
	$(CC) $(CFLAGS) -c stack.c

murmur3.o:  murmur3.c murmur3.h
	$(CC) $(CFLAGS) -c murmur3.c

top.o:  top.c top.h
	$(CC) $(CFLAGS) -c top.c
	

deletion.o:  deletion.c deletion.h
	$(CC) $(CFLAGS) -c deletion.c	

schedule.o:  schedule.c schedule.h
	$(CC) $(CFLAGS) -c  schedule.c -lpthread
	


clean: 
	$(RM) count *.o *~ diffs

testrun_cache:
	  valgrind --tool=cachegrind ./project -q small.work -i small.init 

testrun:
	  valgrind --leak-check=yes ./project -q small.work -i small.init 

testrun2:
	  valgrind --leak-check=full ./project -q test.work -i test.init 


testrun3:
	  valgrind --leak-check=yes ./project -q medium_dynamic.work -i medium_dynamic.init 

test_large:
	 valgrind --leak-check=yes --gen-suppressions=yes ./project -q large_dynamic.work -i large_dynamic.init 

test_static:
	 valgrind --leak-check=yes --gen-suppressions=yes ./project -q medium_static.work -i medium_static.init

test_large_static:
	 valgrind --leak-check=yes --gen-suppressions=yes ./project -q large_static.work -i large_static.init


run:
	time  ./project -q small.work -i small.init 

run2:
	./project -q test.work -i test.init 

run3:
	time ./project -q medium_dynamic.work -i medium_dynamic.init

run_static:
	 time ./project -q medium_static.work -i medium_static.init

run_large:
	time ./project -q large_dynamic.work -i large_dynamic.init 

run_large_static:
	 time  ./project -q large_static.work -i large_static.init

run_tests:
	./test_project

pipe:
	time ./project -q small.work -i small.init > results.txt


