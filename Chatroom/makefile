CC = g++
FLAGS = -std=c++11
COMPILE_FLAGS = $(FLAGS) -c
COMPILE_THREAD_FLAGS = $(FLAGS) -pthread
BUILD_DIR = build

OBJECT_FILES = $(BUILD_DIR)/Client.o $(BUILD_DIR)/Server.o

all:$(BUILD_DIR) Server.out Client.out

# Client.out: $(BUILD_DIR)/Client.o
# 	$(CC) $(BUILD_DIR)/Client.o -o Client.out
Client.out: source/Client.cpp
	$(CC) $(COMPILE_THREAD_FLAGS) source/Client.cpp -o Client.out

Server.out: $(BUILD_DIR)/Server.o
	$(CC) $(BUILD_DIR)/Server.o -o Server.out 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# $(BUILD_DIR)/Client.o: library/Client.h source/Client.cpp
# 	$(CC) $(COMPILE_THREAD_FLAGS) source/Client.cpp -o $(BUILD_DIR)/Client.o

$(BUILD_DIR)/Server.o: library/Server.h source/Server.cpp
	$(CC) $(COMPILE_FLAGS) source/Server.cpp -o $(BUILD_DIR)/Server.o

.PHONY: clean

clean:
	rm *.out
	rm -rf $(BUILD_DIR)	