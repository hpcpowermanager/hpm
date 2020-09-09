#include <cmath>
#include <cstdio>
#include <iostream>

#include <power.h>
#include <msr.h>
#include <utils.h>

#define MSR_RAPL_POWER_UNIT	0x606
#define MSR_PKG_ENERGY_STATUS	0x611
#define MSR_PP0_ENERGY_STATUS	0x639
#define MSR_DRAM_ENERGY_STATUS	0x619

#define MSR_TSC			0x10
#define MSR_PKG_C2_RESIDENCY	0x60D
#define MSR_PKG_C3_RESIDENCY	0x3F8
#define MSR_PKG_C6_RESIDENCY	0x3F9
#define MSR_PKG_C7_RESIDENCY	0x3FA

#define MSR_CORE_C3_RESIDENCY	0x3FC
#define MSR_CORE_C6_RESIDENCY	0x3FD
#define MSR_CORE_C7_RESIDENCY	0x3FE

void power::init() {
	long long result;
	int fd;
	uint32_t max_int = ~((uint32_t) 0);

	gettimeofday(&stamp_before, NULL);

	for (int i=0; i<NPKGS; i++) {
		fd = open_msr(pkg_map(i));

		result = read_msr(fd, MSR_RAPL_POWER_UNIT);
		energy_units[i] = pow(0.5, (double)((result>>8) & 0x1f));

		pkg_before[i] = read_msr(fd, MSR_PKG_ENERGY_STATUS) & max_int;
		pp0_before[i] = read_msr(fd, MSR_PP0_ENERGY_STATUS) & max_int;
		dram_before[i] = read_msr(fd, MSR_DRAM_ENERGY_STATUS) & max_int;

		close_msr(fd);
	}

	for (int i=0; i<NCORES; i++) {
		fd = open_msr(i);

		cc3[i].duration_before = read_msr(fd, MSR_CORE_C3_RESIDENCY);
		cc6[i].duration_before = read_msr(fd, MSR_CORE_C6_RESIDENCY);
		cc7[i].duration_before = read_msr(fd, MSR_CORE_C7_RESIDENCY);
		cc_tsc_before[i] = read_msr(fd, MSR_TSC);

		close_msr(fd);
	}
}

void power::loop() {
	uint64_t time_delta;
	double time_factor;
	double ratio;
	double t;
	uint32_t max_int = ~((uint32_t) 0);

	gettimeofday(&stamp_after, NULL);
	if (stamp_after.tv_sec != stamp_before.tv_sec) {


	time_factor = 1000000.0 * (stamp_after.tv_sec - stamp_before.tv_sec) + stamp_after.tv_usec - stamp_before.tv_usec;
	time_delta = 1000000 * (stamp_after.tv_sec - stamp_before.tv_sec) + stamp_after .tv_usec - stamp_before.tv_usec;

	t = time_delta1(stamp_before, stamp_after);

	for (int i=0; i<NPKGS; i++) {
		int fd = open_msr(pkg_map(i));

		pkg_after[i] = read_msr(fd, MSR_PKG_ENERGY_STATUS) & max_int;
		pkg_energy[i] = calc_power(pkg_before[i], pkg_after[i], t);

		pp0_after[i] = read_msr(fd, MSR_PP0_ENERGY_STATUS) & max_int;
		pp0_energy[i] = calc_power(pp0_before[i], pp0_after[i], t);

		dram_after[i] = read_msr(fd, MSR_DRAM_ENERGY_STATUS) & max_int;
		dram_energy[i] = calc_power(dram_before[i], dram_after[i], t);

		pkg_before[i] = pkg_after[i];
		pp0_before[i] = pp0_after[i];
		dram_before[i] = dram_after[i];

		if (fd)
			close_msr(fd);
	}

	for (int i=0; i<NCORES; i++) {
		int fd = open_msr(i);

		cc_tsc_after[i] = read_msr(fd, MSR_TSC);
		if (cc_tsc_after[i] == cc_tsc_before[i])
			continue;

		ratio = 1.0 * time_delta / (cc_tsc_after[i] - cc_tsc_before[i]);

		cc3[i] = update(cc3[i], fd, MSR_CORE_C3_RESIDENCY, ratio, time_factor);
		cc6[i] = update(cc6[i], fd, MSR_CORE_C6_RESIDENCY, ratio, time_factor);
		cc7[i] = update(cc7[i], fd, MSR_CORE_C7_RESIDENCY, ratio, time_factor);

		cc_tsc_before[i] = cc_tsc_after[i];

		if (fd)
			close_msr(fd);
	}
	stamp_before = stamp_after;
	}
}

idle_state power::update(idle_state state, int fd, int msr, double ratio, double time_factor) {
	state.duration_after = read_msr(fd, msr);
	state.duration_delta = ratio * (state.duration_after - state.duration_before);
	state.pvalue = percentage(state.duration_delta / time_factor);
	state.duration_before = state.duration_after;
	return state;
}

double power::calc_power(uint64_t before, uint64_t after, double time_delta) {
        if (time_delta == 0.0f || time_delta == -0.0f) { return 0.0; }
        double energy = energy_units[0] * ((double) energy_delta(before,after));
        return energy / time_delta;
}

double power::time_delta1(struct timeval begin, struct timeval end) {
        return (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
}

uint64_t power::energy_delta(uint64_t before, uint64_t after){
	uint64_t max_int = ~((uint32_t) 0);
	uint64_t eng_delta = after - before;

	// Check for rollovers
	if (before > after) {
		eng_delta = after + (max_int - before);
	}
	return eng_delta;
}

double power::get_total_power() {
	return pkg_energy[0] + pkg_energy[1];
}

double power::get_pp0_power() {
	return pp0_energy[0] + pp0_energy[1];
}

double power::get_dram_power() {
	return dram_energy[0] + dram_energy[1];
}

double power::get_percent_cstate() {
	double total_cstate = 0;
	for (int i=0; i<NCORES; i++)
		total_cstate += cc3[i].pvalue + cc6[i].pvalue + cc7[i].pvalue;
	percent_cstate = total_cstate / NCORES;
	return percent_cstate;
}
