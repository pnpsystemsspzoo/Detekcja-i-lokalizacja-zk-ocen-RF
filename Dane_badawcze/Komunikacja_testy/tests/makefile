CXX = g++
INCLUDES= -I./ -I../sources/ -I../libs/
CXXFLAGS = -g $(INCLUDES) -std=c++11
SRCM= $(shell find ./ ../sources/ ! -name "main.cpp" -name "*.cpp")
#$(info $$SRCM is [${SRCM}])
OBJM = $(SRCM:.cpp=.o)
FFLL_LIB=$(shell find ../libs/ffll -name "ffll.a")
LINKFLAGS= -lcppunit -lssl -lcrypto $(FFLL_LIB)
run-soft-tests: runner.cpp $(OBJM)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJM) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)
clean:
	rm -f $(OBJM) ./run-soft-tests ./soft-tests-results.xml ./a.out
# Default compile

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@


