EXE ?= null
CC ?= null

all:
	cd Schoenemann && clang++ -Wall -std=c++20 Schoenemann.cpp search.cpp timeman.cpp helper.cpp psqt.cpp tt.cpp moveorder.cpp datagen/gen.cpp -o ../$(EXE).exe