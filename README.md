SHREC - Simultaneous Hardcore Real-time Editing of Code
=================

Collaborative coding application - SoftSys Fall '12

We implemented a Google Docs-style application that allows for concurrent editing of a ﬁle.
Our application is speciﬁcally tailored for code and includes code-speciﬁc features like syntax
highlighting. Our GUI, written in C++ and Qt, allows a user to write code which then gets sent
to a server. Multiple users can connect to the same server to allow them to edit the same body
of code together and in real time.

The client folder contains all of the files for the clientside gui and networking. mainwindow.cpp is for the gui, highlighter.cpp is for syntax highlighting and client.cpp handles networking and launching the gui.

The server folder contains server.cpp which handles both communication with the client and the very simple server gui.

To make each respective executable, run "make client" or "make server" in the corresponding folder.
If that does not work, delete the Makefile, the .pro files and the moc_ files. Run "qmake -project" and then "qmake -makefile".
