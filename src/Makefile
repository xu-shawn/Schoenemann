EXE ?= null
CC ?= null

all:
	cmake . > cmake_output.txt
	cmake --build . --config Release >> cmake_output.txt
	$(eval TARGET = $(shell py find_exe.py cmake_output.txt))
	cmd /C move /Y $(TARGET) src/$(EXE).exe