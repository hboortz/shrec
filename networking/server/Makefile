server: server.cpp server.h
	clang -c server.cpp -o server.o `pkg-config --cflags --libs QtCore QtGui QtNetwork`

main: server.cpp server.h main.cpp
	clang -o server main.cpp `pkg-config --cflags --libs QtCore QtGui QtNetwork`

