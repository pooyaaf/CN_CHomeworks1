
all: server client

server: server/main.cpp server/commands/*.hpp server/Application.hpp utility.h
	g++ -o server.out  server/main.cpp server/context/Path.cpp \
	server/Application.cpp server/context/AppContext.cpp utility.c

client: Extra.h
	g++ -o client.out Client.cpp Extra.cpp utility.c