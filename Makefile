EXE ?= null
CC ?= null

all:
	cd Schoenemann && clang++ -Wall -O3 -std=c++20 Schoenemann.cpp search.cpp timeman.cpp helper.cpp tt.cpp main.cpp moveorder.cpp datagen/gen.cpp -o ../src/$(EXE).exe