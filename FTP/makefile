all: server.out client.out

server.out: server/main.cpp server/Application.hpp server/commands/* utility.hpp server/Messages.h server/commands/control.hpp
	g++ -pthread -o server.out server/main.cpp server/context/Path.cpp \
	server/Application.cpp server/context/AppContext.cpp utility.cpp \
	server/commands/user.cpp server/commands/file_system.cpp server/commands/logging.cpp server/commands/control.cpp

client.out: server/Messages.h utility.hpp Client.cpp
	g++ -o client.out Client.cpp utility.cpp

clean:
	rm server.out client.out