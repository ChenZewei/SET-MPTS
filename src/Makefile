
CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT -fPIE $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -g -D_REENTRANT -fPIE $(DEFINES) -std=c++11 -std=gnu++11 -fext-numeric-literals
INCPATH       = -I. -I../include -I../include/SchedTest -I../include/QCustomPlot -I../include/mathGL -I../include/LP
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = -lgmp -lgmpxx -lmgl -lpthread 
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
DEL_FILE      = rm -f
DEL_DIR       = rmdir
MOVE          = mv -f
MKDIR         = mkdir -p
TARGET	      = SchedulingAnalysis

SchedulingAnalysis:scheduling_test.o tasks.o processors.o tinyxml2.o random_gen.o mgl_chart.o resources.o xml.o param.o output.o
	$(CXX) $(CXXFLAGS) $(INCPATH) -o $(TARGET) scheduling_test.o tasks.o processors.o tinyxml2.o random_gen.o mgl_chart.o resources.o xml.o param.o output.o $(LFLAGS) $(LIBS)
	
scheduling_test.o:scheduling_test.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c scheduling_test.cpp $(LIBS)

tasks.o:tasks.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c tasks.cpp $(LIBS)

processors.o:processors.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c processors.cpp $(LIBS)

random_gen.o:random_gen.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c random_gen.cpp $(LIBS)

resources.o:resources.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c resources.cpp $(LIBS)

tinyxml2.o:tinyxml2.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c tinyxml2.cpp $(LIBS)

mgl_chart.o:mathGL/mgl_chart.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c mathGL/mgl_chart.cpp $(LIBS)

xml.o:xml.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c xml.cpp $(LIBS)

param.o:param.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c param.cpp $(LIBS)

output.o:output.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c output.cpp $(LIBS)

clean:
	rm -f *.o ${ALL}	

