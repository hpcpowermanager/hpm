#ifndef STAT_H
#define STAT_H

class procstat {
	unsigned long long lastTotalUser;
	unsigned long long lastTotalUserLow;
	unsigned long long lastTotalSys;
	unsigned long long lastTotalIdle;
	unsigned long long lastTotalIowait;
	unsigned long long lastTotalIrq;
	unsigned long long lastTotalSoftirq;
	double percent_cpu;
	double percent_idle;
	double percent_io;
    public:
	void init();
	void loop();
	double get_percent_cpu();
	double get_percent_idle();
};

#endif
