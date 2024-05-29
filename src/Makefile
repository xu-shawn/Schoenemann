EXE ?= null
CC ?= null

all:
	cd ../Schoenemann && clang++ -fsanitize=address -Wall -O3 -std=c++20 Schoenemann.cpp evaluate.cpp search.cpp timeman.cpp psqt.cpp file.cpp tt.cpp moveorder.cpp datagen/gen.cpp nnue.cpp -o ../src/$(EXE).exe