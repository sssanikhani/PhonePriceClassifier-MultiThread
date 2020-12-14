CXX = g++
CXX_STD_FLAG = -std=c++11
CXX_OBJ_FLAG = -c
CXX_EXE_FLAG = -o

UTILS = utils.cpp

MAIN = main.cpp

OBJ_FILES = main.o\
			utils.o


EXE_FILE = PhonePricePrediction.out
EXE_ARGS = datasets/


target:

	$(CXX) $(CXX_STD_FLAG) $(CXX_OBJ_FLAG) $(UTILS)
	
	$(CXX) $(CXX_STD_FLAG) $(CXX_OBJ_FLAG) $(MAIN)

	$(CXX) $(CXX_STD_FLAG) $(CXX_EXE_FLAG) $(EXE_FILE) $(OBJ_FILES)

run:

	$(CXX) $(CXX_STD_FLAG) $(CXX_OBJ_FLAG) $(UTILS)
	
	$(CXX) $(CXX_STD_FLAG) $(CXX_OBJ_FLAG) $(MAIN)

	$(CXX) $(CXX_STD_FLAG) $(CXX_EXE_FLAG) $(EXE_FILE) $(OBJ_FILES)

	./$(EXE_FILE) $(EXE_ARGS)

clean:
	rm -f $(OBJ_FILES)
	rm -f $(EXE_FILE)

