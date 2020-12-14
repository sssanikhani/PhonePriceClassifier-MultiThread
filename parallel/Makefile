CXX = g++
CXX_FLAGS = -std=c++11
LINK_FLAGS = -pthread

OBJ_FILES = main.o \
			utils.o

EXE_FILE = PhonePricePrediction.out
EXE_ARGS = datasets/

CPP_FILES = main.cpp \
			utils.cpp

target:

	$(CXX) $(CXX_FLAGS) $(LINK_FLAGS) -c $(CPP_FILES)

	$(CXX) $(CXX_FLAGS) $(LINK_FLAGS) -o $(EXE_FILE) $(OBJ_FILES)

run:

	$(CXX) $(CXX_FLAGS) $(LINK_FLAGS) -c $(CPP_FILES)

	$(CXX) $(CXX_FLAGS) $(LINK_FLAGS) -o $(EXE_FILE) $(OBJ_FILES)

	./$(EXE_FILE) $(EXE_ARGS)

clean:

	rm -f *.o
	rm -f *.out