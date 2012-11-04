hello: hello.cpp
		g++ `pkg-config --cflags --libs QtCore QtGui` -o hello hello.cpp

