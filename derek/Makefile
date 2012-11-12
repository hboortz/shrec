client:		client.cpp
	clang -m64 -o client client.cpp `pkg-config --cflags --libs QtCore QtGui glib-2.0`
server:		server.cpp
	clang -m64 -o server server.cpp `pkg-config --cflags --libs QtCore QtGui glib-2.0`
onemoretry:	onemoretry.cpp
	g++ -m64 -o onemoretry onemoretry.cpp `pkg-config --cflags --libs QtCore QtGui`
