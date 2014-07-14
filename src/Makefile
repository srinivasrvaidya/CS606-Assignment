output: main.o Utility.o
	gcc -o "$@" main.o Utility.o -lGL -lGLU -lglut -lc -lm -lstdc++
quaternion_assignment2.o: main.cpp
	gcc -c main.cpp -lGL -lGLU -lglut -lc -lm -lstdc++ 
Utility.o: Utility.cpp
	g++ -c Utility.cpp -lglut -lGL -lGLU -lc -lm -lstdc++ 
clean:
	rm *.o
