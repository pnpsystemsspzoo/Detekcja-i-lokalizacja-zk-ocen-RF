CXX = g++
INCLUDES= -I./
CXXFLAGS = -g $(INCLUDES) -std=c++11 -pthread
SRCM= $(shell find ./ -name "*.cpp")
#$(info $$SRCM is [${SRCM}])
OBJM = $(SRCM:.cpp=.o)
LINKFLAGS= -pthread -lssl -lcrypto
app: main.cpp $(OBJM)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJM) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)
clean:
	rm -f $(OBJM) ./app
# Default compile

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@


