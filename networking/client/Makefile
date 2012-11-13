moc_%.cpp: %.h
	moc $(DEFINES) $(INCPATH) $< -o $@

client: client.cpp client.h
	clang -o client client.cpp `pkg-config --cflags --libs QtCore QtGui QtNetwork`
