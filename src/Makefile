
CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT -fPIE $(DEFINES) -fpermissive
CXXFLAGS      = -pipe -O2 -Wall -W -g -D_REENTRANT -fPIE $(DEFINES) -std=c++11 -std=gnu++11 -fext-numeric-literals -fpermissive
INCPATH       = -I. -I../include -I../include/SchedTest -I../include/SchedTest/global -I../include/SchedTest/partitioned -I../include/QCustomPlot -I../include/mathGL -I../include/LP -I../include/SchedTest/blocking/lp
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = -lgmp -lgmpxx -lmgl -lpthread -lglpk
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
DEL_FILE      = rm -f
DEL_DIR       = rmdir
MOVE          = mv -f
MKDIR         = mkdir -p
TARGET	      = SchedulingAnalysis

SchedulingAnalysis:scheduling_test.o tasks.o processors.o tinyxml2.o random_gen.o mgl_chart.o resources.o xml.o param.o output.o lp.o solution.o varmapper.o lp_dpcp.o lp_pip.o lp_fmlp.o ilp_spinlock.o sched_test_base.o sched_test_factory.o p_sched.o g_sched.o pfp_gs.o rta_gfp_native.o rta_gfp_bc.o rta_gfp_ng.o rta_pfp_wf.o rta_pfp_wf_spinlock.o rta_pfp_wf_semaphore.o  
	$(CXX) $(CXXFLAGS) $(INCPATH) -o $(TARGET) scheduling_test.o tasks.o processors.o tinyxml2.o random_gen.o mgl_chart.o resources.o xml.o param.o output.o lp.o solution.o varmapper.o lp_dpcp.o lp_pip.o lp_fmlp.o ilp_spinlock.o sched_test_base.o sched_test_factory.o p_sched.o g_sched.o pfp_gs.o rta_gfp_native.o rta_gfp_bc.o rta_gfp_ng.o rta_pfp_wf.o rta_pfp_wf_spinlock.o rta_pfp_wf_semaphore.o  $(LFLAGS) $(LIBS)
	
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

lp.o:LP/lp.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c LP/lp.cpp $(LIBS)

solution.o:LP/solution.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c LP/solution.cpp $(LIBS)

varmapper.o:LP/varmapper.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c LP/varmapper.cpp $(LIBS)

lp_dpcp.o:SchedTest/blocking/lp/lp_dpcp.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/blocking/lp/lp_dpcp.cpp $(LIBS)

lp_pip.o:SchedTest/blocking/lp/lp_pip.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/blocking/lp/lp_pip.cpp $(LIBS)

lp_fmlp.o:SchedTest/blocking/lp/lp_fmlp.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/blocking/lp/lp_fmlp.cpp $(LIBS)

ilp_spinlock.o:SchedTest/blocking/lp/ilp_spinlock.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/blocking/lp/ilp_spinlock.cpp $(LIBS)

sched_test_base.o:SchedTest/sched_test_base.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/sched_test_base.cpp $(LIBS)

sched_test_factory.o:SchedTest/sched_test_factory.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/sched_test_factory.cpp $(LIBS)

p_sched.o:SchedTest/partitioned/p_sched.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/partitioned/p_sched.cpp $(LIBS)

g_sched.o:SchedTest/global/g_sched.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/global/g_sched.cpp $(LIBS)

pfp_gs.o:SchedTest/pfp_gs.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/pfp_gs.cpp $(LIBS)

rta_gfp_native.o:SchedTest/global/rta_gfp_native.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/global/rta_gfp_native.cpp $(LIBS)

rta_gfp_bc.o:SchedTest/global/rta_gfp_bc.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/global/rta_gfp_bc.cpp $(LIBS)

rta_gfp_ng.o:SchedTest/global/rta_gfp_ng.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/global/rta_gfp_ng.cpp $(LIBS)

rta_pfp_wf.o:SchedTest/partitioned/rta_pfp_wf.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/partitioned/rta_pfp_wf.cpp $(LIBS)

rta_pfp_wf_spinlock.o:SchedTest/partitioned/rta_pfp_wf_spinlock.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/partitioned/rta_pfp_wf_spinlock.cpp $(LIBS)

rta_pfp_wf_semaphore.o:SchedTest/partitioned/rta_pfp_wf_semaphore.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c SchedTest/partitioned/rta_pfp_wf_semaphore.cpp $(LIBS)

clean:
	rm -f *.o ${ALL}	

all:
	make clean
	make

