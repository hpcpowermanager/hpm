#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sys/time.h>
#include <thread>
#include <csignal>

#include <power.h>
#include <procstat.h>
#include <utils.h>

#include <cpucounters.h>

using namespace std;
using namespace pcm;

PCM *hpcm;

void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  
   hpcm->cleanup();
   exit(signum);  
}


void run_measurements(power pwr, procstat st, SystemCounterState before_sstate1) {
	SystemCounterState before_sstate = before_sstate1;
	while (true) {
		sleep(1);
		time_t now;
		time(&now);
		struct tm *tinfo;
		tinfo = localtime(&now);

		pwr.loop();
		st.loop();

		SystemCounterState after_sstate = getSystemCounterState();
		double mem = (double(getBytesReadFromMC(before_sstate,after_sstate) + getBytesWrittenToMC(before_sstate,after_sstate)) ) / (1e6);

		write_samples(tinfo, pwr, st, mem);

		//before_sstate = after_sstate;
		before_sstate = move(after_sstate);
	}
}

int main (int argc, char** argv) {
	// register signal SIGINT and signal handler  
	signal(SIGINT, signalHandler);
	int status;
	pid_t pid = fork();
	if (pid < 0) {
		std::cout<<"fork() failed!\n";
		exit(1);
	} else if (pid == 0) {
		if (execvp(argv[1], argv+1) < 0) {
			std::cout<<"Execvp failed!\n";
			exit(1);
		}
	} else {
		power pwr;
		procstat st;

		hpcm = PCM::getInstance();
		PCM::ErrorCode returnResult = hpcm->program(PCM::DEFAULT_EVENTS, NULL);

		if (returnResult != PCM::Success){
			std::cerr << "Intel's PCM couldn't start" << std::endl;
			std::cerr << "Error code: " << returnResult << std::endl;
			exit(1);
		}

		write_headers();
		pwr.init();
		st.init();
		SystemCounterState before_sstate = getSystemCounterState();

		std::thread thread_measurements(run_measurements, pwr, st, before_sstate);
		while (wait(&status) != pid);

		hpcm->cleanup();
		thread_measurements.detach();
	}
	return 0;
}

