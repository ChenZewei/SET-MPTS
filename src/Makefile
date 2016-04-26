
RMtest:scheduling_test.o tasks.o RMS.o processors.o
	g++ -std=c++11 -o RMtest scheduling_test.o tasks.o RMS.o processors.o -lgmpxx -lgmp

scheduling_test.o:scheduling_test.cpp
	g++ -std=c++11 -c scheduling_test.cpp

tasks.o:tasks.cpp
	g++ -std=c++11 -c tasks.cpp

RMS.o:./SchedTest/RMS.cpp
	g++ -std=c++11 -c ./SchedTest/RMS.cpp

processors.o:processors.cpp
	g++ -std=c++11 -c processors.cpp
