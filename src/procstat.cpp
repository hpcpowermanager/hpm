#include <iostream>
#include <fstream>

#include <procstat.h>

void procstat::init(){
	/*FILE* file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu %llu %llu%llu", &lastTotalUser, &lastTotalUserLow, &lastTotalSys, &lastTotalIdle, &lastTotalIowait, &lastTotalIrq, &lastTotalSoftirq);
	fclose(file);*/

	std::ifstream file("/proc/stat");
	file.ignore(5, ' '); // Skip the 'cpu' prefix.
	file >> lastTotalUser >> lastTotalUserLow >> lastTotalSys >> lastTotalIdle >> lastTotalIowait >> lastTotalIrq >> lastTotalSoftirq;
	file.close();
}

void procstat::loop(){
	//FILE* file;
	unsigned long long totalUser, totalUserLow, totalSys, totalIdle, totalIowait, totalIrq, totalSoftirq, total;

	/*file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu", &totalUser, &totalUserLow, &totalSys, &totalIdle, &totalIowait, &totalIrq, &totalSoftirq);
	fclose(file);*/

	std::ifstream file("/proc/stat");
	file.ignore(5, ' '); // Skip the 'cpu' prefix.
	file >> totalUser >> totalUserLow >> totalSys >> totalIdle >> totalIowait >> totalIrq >> totalSoftirq;
	file.close();

	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow || 
		totalSys < lastTotalSys || totalIdle < lastTotalIdle){
		//Overflow detection. Just skip this value.
		percent_cpu = -1.0;
	}
	else{
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
		percent_cpu = total;
		total += (totalIdle - lastTotalIdle);
		percent_cpu /= total;
		percent_cpu *= 100;

		total = (totalUser-lastTotalUser) + (totalUserLow-lastTotalUserLow) + (totalSys-lastTotalSys) + (totalIdle-lastTotalIdle) + (totalIowait-lastTotalIowait) + (totalIrq-lastTotalIrq) + (totalSoftirq-lastTotalSoftirq);
		percent_idle = (totalIdle - lastTotalIdle) * 100;
		percent_idle /= total;


		// Calculating io percent
		percent_io = ((totalIowait - lastTotalIowait) * 100) / total;
	}

	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;
	lastTotalIowait = totalIowait;
	lastTotalIrq = totalIrq;
	lastTotalSoftirq = totalSoftirq;
}

double procstat::get_percent_cpu() {
	return percent_cpu;
}

double procstat::get_percent_idle() {
	return percent_idle;
}
