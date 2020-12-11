CXX = g++
CXXFLAGS = -Wall -g
SRC = main.cpp Grammar.cpp
OBJ = $(SRC:.cpp=.o)

prog: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

ifneq ($(MAKECMDGOALS), clean)
-include deps.mk
endif

deps.mk: $(SRC)
	$(CXX) $(CXXFLAGS) -MM $^ > deps.mk
clean:
	rm -f *.o prog deps.mk
