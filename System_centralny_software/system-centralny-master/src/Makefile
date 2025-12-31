CXX = g++
INCLUDES= -I./ -I../external/
CXXFLAGS = -g $(INCLUDES) -Wall -std=c++11 -fpermissive -pthread -lcrypto -lssl
SRCM= $(shell find ./ -name "*.cpp")
OBJM = $(SRCM:.cpp=.o)
LINKFLAGS= -lpq -pthread
SCapp: main.cpp $(OBJM)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJM) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)
clean:
	rm -f $(OBJM) ./SCapp
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

