all:
	make matrixadd

matrixadd: matrixadd.o
	g++ -o matrixadd matrixadd.o
	
matrixadd.o: matrixadd.cc
	g++ -c matrixadd.cc
	

clean:
	rm -f matrixadd.o