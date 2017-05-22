/* Little Init by Alexander Richards
   under MIT; Please compile staticly */
//Include stuff
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

#define MAX_BUF 1024
// #include <time.h>
using namespace std;
// Declare Vars
float version = 0.1;
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

/* void change_runlevel(int new_runlevel) {
cout << "Entering runlevel " << new_runlevel << endl;
std::stringstream Stream;
Stream.str("sh /etc/init.d/rc ");
Stream << new_runlevel;
rlevel_char = Stream.str().c_str();
std::system(rlevel_char);
runlevel = new_runlevel;
} */ //Does not work yet, what a shame

/* void loop() {
while(true){
// TODO: Runlevels; More commands
// Wait for messages
print("Here1"); //Debug
fd = open(fifo_pipe, O_RDONLY);
read(fd, buf, MAX_BUF);
if (atoi(buf) != 0) {
change_runlevel(atoi(buf));
}
close(fd);
}
print("Here2"); //Debug
} */ // Still aint working
inline bool exist(const std::string& name) {
ifstream file(name);
if (file.good()) {return true;} else {return false;}} // Simple file checker; This line is probably not required but why not
void init() {
print("Remounting Root as Read-Write!");
std::system("mount -o remount,rw /");
print("Entering Runlevel 2...");
print("Bringing up processes!");
start_exit_code = std::system("sh /etc/init.d/rc 2");
noend_print("rc script run, exit code "); cout << start_exit_code / 256 << endl;
// cout << "Im here!" << endl; //Debug
done = 1;
runlevel = 2;
if (exist("/linit/c_start.sh")) {
print("Custom start script exists; Also starting it");
std::system("/linit/c_start.sh");
}
// cout << "Im here!" << endl; //Debug
//std::thread loop_thread (loop);
// cout << "Im here!" << endl; //Debug
//loop_thread.detach();
//cout << "Started message thread, ID " << loop_thread.get_id() << endl;
//loop_thread.join();
// cout << "Im here!" << endl; //Debug

while(true){}
}

// Call said functions
int main() {
noend_print("Little Init v"); cout << version; print(" Booting!");
print("");
init();
}
