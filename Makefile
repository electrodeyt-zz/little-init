make: little-init.c++
	g++ --static -o init little-init.c++ -std=c++14
	g++ --static -o ltelinit ltelinit.c++

clean:
	-rm init 2> /dev/null; true
	-rm ltelinit 2> /dev/null; true

install:
	sudo cp ltelinit /bin/ltelinit
	-@echo To install the init run "make install-init", this will delete /sbin/init!

install-init:
	-sudo rm /sbin/init
	sudo cp init /sbin/init
