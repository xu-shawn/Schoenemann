EXE ?= null
CC ?= null

all:
	cd Schoenemann && clang++ -Wall -O3 -std=c++20 schoenemann.cpp search.cpp timeman.cpp helper.cpp tt.cpp main.cpp moveorder.cpp -o ../src/$(EXE).exe