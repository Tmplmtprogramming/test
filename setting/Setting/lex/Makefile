all : main.cpp 
	g++ -c main.cpp -g  -I./
	g++ -c parser.cpp -g  -I./
	g++ -o lex  main.o parser.o -g

clean :
	rm -rf *.o lex 
