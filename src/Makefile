
test:test.o tasks.o RMS.o processors.o
	g++ -o test test.o tasks.o RMS.o processors.o -lgmpxx -lgmp

test.o:test.cpp
	g++ -c test.cpp

tasks.o:tasks.cpp
	g++ -c tasks.cpp

RMS.o:./SchedTest/RMS.cpp
	g++ -c ./SchedTest/RMS.cpp

processors.o:processors.cpp
	g++ -c processors.cpp
