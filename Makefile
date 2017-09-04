#Compiler
CC=g++ -std=c++11 -O3 -Wall

all : directories sharedobjects executables
.PHONY : all

directories: bin build
.PHONY: directories

bin:
	mkdir -p bin
build:
	mkdir -p build

sharedobjects : build/sequence.o build/walk.o build/resample.o
.PHONY : sharedobjects

executables : bin/seres-resample bin/seres-translate
.PHONY : executables

#Link the executables
translate_objects = build/seres-translate.o build/sequence.o build/walk.o build/resample.o 
bin/seres-translate : $(translate_objects)
	$(CC) $(translate_objects) -o $@

resample_objects = build/seres-resample.o build/sequence.o build/walk.o build/resample.o 
bin/seres-resample : $(resample_objects)
	$(CC) $(resample_objects) -o $@

#Object files for executables
build/seres-resample.o : src/seres-resample.cpp
	$(CC) -c src/seres-resample.cpp -o $@
build/seres-translate.o : src/seres-translate.cpp
	$(CC) -c src/seres-translate.cpp -o $@

#Shared object files
build/sequence.o : src/sequence.cpp src/sequence.hpp
	$(CC) -c src/sequence.cpp -o $@
build/walk.o : src/walk.cpp src/walk.hpp
	$(CC) -c src/walk.cpp -o $@
build/resample.o : src/resample.cpp src/resample.hpp
	$(CC) -c src/resample.cpp -o $@


.PHONY : clean
clean :
	rm bin/* build/*
