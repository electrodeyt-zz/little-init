/* Little Init by Alexander Richards
   under MIT; Please compile staticly */
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
#include <fstream>
#include <string.h>

#define MAX_BUF 1024
// #include <time.h>
using namespace std;
// Declare Vars
float version = 0.2;
int start_exit_code;
int done = 0;
int shell_exit;
int fd;
char * fifo_pipe = "/tmp/init_fifo";
char buf[MAX_BUF];
int runlevel;
const char * rlevel_char;

// Define Functions
void print(string msg) {
cout << msg << endl;
}

void noend_print(string msg) {
cout << msg;
}

// https://www.systutorials.com/5510/catching-the-signal-sent-by-kill-in-c-on-linux/ Please don't sue me
void sighandler(int signum, siginfo_t *info, void *ptr)
{
    if (signum == SIGTERM) {
      sync();
      reboot(RB_POWER_OFF);
    } else if (signum == SIGHUP) {
      sync();
      reboot(RB_AUTOBOOT);
    }
}

void catch_sig()
{
    static struct sigaction _sigact;
    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sighandler;
    _sigact.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &_sigact, NULL);
}
//END
inline bool exist(const std::string& name) {
ifstream file(name);
if (file.good()) {return true;} else {return false;}} // Simple file checker; This line is probably not required but why not
void init() {
print("Remounting Root as Read-Write!");
std::system("mount -o remount,rw /");
print("Entering Runlevel 3...");
print("Bringing up processes!");
start_exit_code = std::system("sh /etc/init.d/rc 3");
noend_print("rc script run, exit code "); cout << start_exit_code / 256 << endl;
done = 1;
runlevel = 3;
if (exist("/linit/c_start.sh")) {
print("Custom start script exists; Also starting it");
std::system("/linit/c_start.sh");
}

while(true){}
catch_sig();
}

// Call said functions
int main() {
noend_print("Little Init v"); cout << version; print(" Booting!");
print("");
init();
}
