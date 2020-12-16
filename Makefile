all: Amount_counter

Amount_counter: test.o
	g++ test.o -o Amount_counter

test.o: test.cpp
	g++ -c test.cpp -o test.o 

clean:
	rm -rf *.o Amount_counter
