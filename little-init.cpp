/* Little Init by Alexander Richards
   under MIT; Please compile staticly */

///TODO:
///  "init rl" runlevel-switching

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

#define MAX_BUF 1024

// #include <time.h>
using namespace std;

// Declare Vars
int done = 0; // is init procedure complete?
int runlevel = 1; // runlevel, 0 should always be shutdown
string version = "simple-init v0.3-git";
string rc_script = "/etc/init.d/rc";
// not the final solution, but its the best thing for now, there are more important things :)

// https://www.systutorials.com/5510/catching-the-signal-sent-by-kill-in-c-on-linux/ please don't sue me
void sighandler(int signum, siginfo_t *info, void *ptr) { // so i can turn it off gracefully for now
    	if (signum == SIGTERM) {
      		sync();
      		reboot(RB_POWER_OFF);
    	} else if (signum == SIGHUP) {
      		sync();
      		reboot(RB_AUTOBOOT);
    	}
}

void catch_sig() {
 	static struct sigaction _sigact;
    	memset(&_sigact, 0, sizeof(_sigact));
   	_sigact.sa_sigaction = sighandler;
   	_sigact.sa_flags = SA_SIGINFO;
   	sigaction(SIGTERM, &_sigact, NULL);
   	sigaction(SIGHUP, &_sigact, NULL);
    signal(SIGCHLD, SIG_IGN);
}
//END

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
  call_rc(9); // call single user first
  call_rc(2);
	done = 1;
	runlevel = 3; // still have to implement switching it
	if(exist("/etc/init.d/shell")) {
		pid_t pid = fork();
		if(pid == 0) { // if child process, run the "shell" symlink, hopefully :)
			execl("/etc/init.d/shell", "/etc/init.d/shell", (char*) 0);
			this_thread::sleep_for(std::chrono::milliseconds(1000));
      kill(getpid(), SIGKILL);
		}
	}
}


// Call said functions
int main() {
	cout << version << endl;
  if(getpid() == 1) {
    init();
    while(true) { // do stuff
      if(exist("/tmp/runlevel")) {
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
          }
        }
        file.close();
      }
    }
  } else {
    // should be reserved for runlevel changes
  }
}
