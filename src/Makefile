
test:scheduling_test.o tasks.o processors.o tinyxml2.o
	g++ -std=c++11 -o test scheduling_test.o tasks.o processors.o tinyxml2.o -lgmpxx -lgmp

scheduling_test.o:scheduling_test.cpp
	g++ -std=c++11 -c scheduling_test.cpp

tasks.o:tasks.cpp
	g++ -std=c++11 -c tasks.cpp

processors.o:processors.cpp
	g++ -std=c++11 -c processors.cpp

tinyxml2.o:tinyxml2.cpp
	g++ -std=c++11 -c tinyxml2.cpp

clean:
	rm test
	rm *.o

	
