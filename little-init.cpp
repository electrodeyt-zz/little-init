/* Little Init by Alexander Richards
   under MIT*/

///TODO:
/// /run/initctl fifo compatability

//Include stuff
#include <signal.h>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sstream>
#include <thread>
#include <chrono>
#include <fstream>
#include <string.h>

// #include <time.h>
using namespace std;

// Declare Vars
int done = 0; // is init procedure complete?
int runlevel = 5; // runlevel, 0 should always be shutdown, default is 5
pid_t shell_pid;
string version = "little-init v0.3";
string rc_script = "/etc/init.d/rc";
//const char* initctl = "/run/initctl"; // for later

void call_rc(int rl) {
  if(rl == 9) {
    system(string(rc_script + string(" S")).c_str());
  } else {
    system(string(rc_script + string(" ") + to_string(rl)).c_str());
  }
}

inline bool exist(const std::string& name) {
	ifstream file(name);
	if (file.good()) {return true;} else {return false;}
} // Simple file checker; This line is probably not required but why not

void init() {
  system("mount -a"); // just mount everything now
	system("mount -o remount,rw /"); // remount root as rw, just in case its ro
  signal(SIGCHLD, SIG_IGN); // make kernel reap zombies automatically
  call_rc(9); // call single user first
  call_rc(runlevel); // call default runlevel/set runlevel
  //mkfifo(initctl, 0666); // make pipe
	done = 1;
	if(exist("/etc/init.d/shell") || runlevel == 9) { // run a shell if single user
		pid_t pid = fork();
		if(pid == 0) { // if child process, run the "shell" symlink, hopefully :)
			execl("/etc/init.d/shell", "/etc/init.d/shell", (char*) 0); // run shell
			this_thread::sleep_for(std::chrono::milliseconds(1000));
      kill(getpid(), SIGKILL); // really should not be here, unless it is not executable, so kill self
		} else {
      shell_pid = pid;
    }
	}
}


// Call said functions
int main(int argc, char** argv) {
  if(getpid() == 1) { // if running as init, do init stuff
	  cout << version << endl;
    if(argc != 1) { // if runlevel is specified
      runlevel = atoi(argv[1]);
      if(string(argv[1]) == "s" || string(argv[1]) == "S") {
        runlevel = 9;
      }
    }
    init();
    while(true) { // listen for stuff
      if(exist("/tmp/runlevel")) { // if this file exists, read it and switch to its runlevel
        int tmp_runlevel = 0; // runlevel from /tmp/runlevel. default = 0
        string temp;
        ifstream file("/tmp/runlevel");
        if(file.is_open()) {
          getline(file, temp);
          // temp should have the runlevel in it. 9 is alias for S
          if(temp == "") {
            // nothing, continue
          } else {
            tmp_runlevel = stoi(temp);
            call_rc(tmp_runlevel);
            runlevel = tmp_runlevel;
          }
        }
        file.close();
      }
      this_thread::sleep_for(std::chrono::milliseconds(1000)); // dont use the cpu too much :)
    }
  } else {
    // not running as init, switch runlevel
    if(argc == 1) {
      cout << "Usage: init <runlevel>";
    } else {
      runlevel = atoi(argv[1]);
      ofstream file("/tmp/runlevel");
      file << runlevel;
      file.close();
    }
  }
}
