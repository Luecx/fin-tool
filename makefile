EXE = fin-tool
SRC = src/main.cpp
CC = clang++
DEFS = -DNDEBUG

STD = -std=c++17
LIBS =
WARN = -Wall -Wextra -Wshadow

FLAGS = $(STD) $(WARN) -g -O3 -flto $(DEFS)

all:
	$(CC) $(FLAGS) $(SRC) $(LIBS) -o $(EXE)
