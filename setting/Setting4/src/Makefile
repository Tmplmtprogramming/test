all : main.cpp element.cpp structure.cpp
	g++ -c -O3 main.cpp -g  -I./
	g++ -c -O3 element.cpp -g  -I./
	g++ -c -O3 structure.cpp -g  -I./
	g++ -o setting4 -O3 main.o element.o structure.o -g

clean :
	rm -rf *.o setting4 
