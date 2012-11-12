server: server.cpp server.h
	clang `pkg-config --cflags --libs QtCore QtGui QtNetwork` -c server.cpp -o server.o

main: server.cpp server.h main.cpp
	clang `pkg-config --cflags --libs QtCore QtGui QtNetwork` -o server main.cpp 

