
CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = 
CXXFLAGS      =  -std=c++11 -std=gnu++11 -fext-numeric-literals
INCPATH       = -I. -I../include -I../include/SchedTest -I../include/QCustomPlot -I/usr/local/lib
LIBS          = -lgmp -lgmpxx -lmgl

test:scheduling_test.o tasks.o processors.o tinyxml2.o
	$(CXX) $(CXXFLAGS) $(INCPATH) -o test scheduling_test.o tasks.o processors.o tinyxml2.o $(LIBS)

scheduling_test.o:scheduling_test.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c scheduling_test.cpp $(LIBS)

tasks.o:tasks.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c tasks.cpp $(LIBS)

processors.o:processors.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c processors.cpp $(LIBS)

tinyxml2.o:tinyxml2.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c tinyxml2.cpp $(LIBS)

clean:
	rm -f *.o ${ALL}	
