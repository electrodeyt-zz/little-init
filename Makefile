make: little-init.cpp
	g++ -o init little-init.cpp -std=c++11 -static
clean:
	-rm init 2> /dev/null; true

install:
	-@echo To install the init run "make install-init", the existing init will be renamed to init.bak

install-init:
	-sudo mv /sbin/init /sbin/init.bak
	sudo cp init /sbin/init
