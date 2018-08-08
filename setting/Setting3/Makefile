all : main.cpp element.cpp structure.cpp
	g++ -c main.cpp -O2 -g  -I./
	g++ -c element.cpp -O2 -g  -I./
	g++ -c structure.cpp -O2 -g  -I./
	g++ -o setting3 main.o element.o structure.o -g -O2 

clean :
	rm -rf *.o setting3 
