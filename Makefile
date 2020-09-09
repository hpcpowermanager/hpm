TARGET = hpm

CXX = g++
CXXFLAGS = -std=c++11 -O0 -g
LDFLAGS = -lm -lrt -lpthread -lpcm -L pcm.so/
INC = -I include/ 

OBJ = obj/

$(TARGET): $(OBJ)hpm.o $(OBJ)msr.o $(OBJ)power.o $(OBJ)procstat.o $(OBJ)utils.o
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -o $@ $^

$(OBJ)hpm.o: src/hpm.cpp 
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -c -o $@ $^

$(OBJ)msr.o: src/msr.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -c -o $@ $^

$(OBJ)power.o: src/power.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -c -o $@ $^

$(OBJ)procstat.o: src/procstat.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -c -o $@ $^

$(OBJ)utils.o: src/utils.cpp
	$(CXX) $(CXXFLAGS) $(INC) $(LDFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJ)*.o $(TARGET)

