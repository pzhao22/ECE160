# this will compile all files everytime make is run
make: main.cpp board.cpp monopoly.h
	g++ -o main main.cpp board.cpp playgame.cpp -I.

# this will clean or remove compiled files so you can start fresh
clean:
	rm -f *.o *.exe
