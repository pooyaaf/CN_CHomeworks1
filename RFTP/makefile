CC = g++
FLAGS = -std=c++11
COMPILE_FLAGS = $(FLAGS) -c
BUILD_DIR = build
SERVER_FILES_DIR = Files

OBJECT_FILES = $(BUILD_DIR)/Client.o $(BUILD_DIR)/Server.o $(BUILD_DIR)/User.o $(BUILD_DIR)/Config.o $(BUILD_DIR)/Command.o

all:$(BUILD_DIR) Server.out Client.out

Client.out: $(BUILD_DIR)/Client.o $(BUILD_DIR)/Config.o $(BUILD_DIR)/User.o
	$(CC) $(BUILD_DIR)/Client.o $(BUILD_DIR)/Config.o $(BUILD_DIR)/User.o -o Client.out

Server.out: $(BUILD_DIR)/Server.o $(BUILD_DIR)/User.o $(BUILD_DIR)/Config.o $(BUILD_DIR)/Command.o
	$(CC) $(BUILD_DIR)/Server.o $(BUILD_DIR)/User.o $(BUILD_DIR)/Config.o $(BUILD_DIR)/Command.o -o Server.out 

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/Client.o: library/Client.h library/Config.h library/User.h source/Client.cpp
	$(CC) $(COMPILE_FLAGS) source/Client.cpp -o $(BUILD_DIR)/Client.o

$(BUILD_DIR)/Server.o: library/Server.h library/Config.h library/Command.h library/User.h source/Server.cpp
	$(CC) $(COMPILE_FLAGS) source/Server.cpp -o $(BUILD_DIR)/Server.o

$(BUILD_DIR)/User.o: library/User.h source/User.cpp
	$(CC) $(COMPILE_FLAGS) source/User.cpp -o $(BUILD_DIR)/User.o

$(BUILD_DIR)/Config.o: library/Config.h library/User.h source/Config.cpp
	$(CC) $(COMPILE_FLAGS) source/Config.cpp -o $(BUILD_DIR)/Config.o

$(BUILD_DIR)/Command.o: library/User.h library/Command.h library/Constant.h source/Command.cpp
	$(CC) $(COMPILE_FLAGS) source/Command.cpp -o $(BUILD_DIR)/Command.o

.PHONY: clean

clean:
	rm *.out
	rm -rf $(BUILD_DIR)	
	rm -rf $(SERVER_FILES_DIR)	