# Author: Mantas Macionis
# License: Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
# Git: https://github.com/mantasmacionis/ccDevLabs/
# Makefile for the WaTor Project

CXX = g++
CPPFILES= wator.cpp
CPPFLAGS= -Wall -fopenmp
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lgomp
# Executable name
EXE= watorsimulation
# Source file 
SRCS=wator.cpp
# Object files (replace .cpp with .o)
OBJS=$(subst .cpp,.o,$(SRCS))

#This rule says that each .o file depends on a .cpp file of the same name
#This is actually built into Make but anyways...
%.o: %.cpp
	$(CXX) -c -o $@ $< $(CPPFLAGS)  $(DEBUGFLAGS)

# Default rule: build the executable using the object files
ALL: $(OBJS)
	$(CXX) $(OBJS) -o $(EXE) $(LDFLAGS)

