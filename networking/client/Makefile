client: client.cpp client.h
	clang `pkg-config --cflags --libs QtCore QtGui` -o client client.cpp

server: server.cpp server.h
	clang `pkg-config --cflags --libs QtCore QtGui QtNetwork` -o server server.cpp 

