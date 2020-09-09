#ifndef POWER_H
#define POWER_H

#include <cstdint>
#include <ctime>
#include <sys/time.h>

#include <config.h>

struct idle_state {
	uint64_t duration_before, duration_after, duration_delta;
	double pvalue;
};

class power {
	double energy_units[NPKGS];
	double pkg_energy[NPKGS], pp0_energy[NPKGS], dram_energy[NPKGS];
	uint64_t pkg_before[NPKGS], pkg_after[NPKGS];
	uint64_t pp0_before[NPKGS], pp0_after[NPKGS];
	uint64_t dram_before[NPKGS], dram_after[NPKGS];

	idle_state cc3[NCORES], cc6[NCORES], cc7[NCORES];

	struct timeval stamp_before, stamp_after;
	uint64_t cc_tsc_before[NCORES], cc_tsc_after[NCORES];
	double time_factor;

	double percent_cstate;

    public:
	void init();
	void loop();
	idle_state update(idle_state state, int fd, int msr, double ratio, double time_factor);

	double time_delta1(struct timeval stamp_before, struct timeval stamp_after);
	uint64_t energy_delta(uint64_t before, uint64_t after);
	double calc_power(uint64_t before, uint64_t after, double time_delta);

	double get_total_power();
	double get_pp0_power();
	double get_dram_power();
	double get_percent_cstate();
};

#endif
