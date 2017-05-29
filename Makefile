make: little-init.cpp
	g++ -o init little-init.cpp -std=c++11
clean:
	-rm init 2> /dev/null; true

install:
	-@echo To install the init run "make install-init", this will delete /sbin/init!

install-init:
	-sudo rm /sbin/init
	sudo cp init /sbin/init
