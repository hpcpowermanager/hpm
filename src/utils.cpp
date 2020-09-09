#include <fstream>

#include <utils.h>
#include <config.h>


int pkg_map(int core) {
	if (core == 0)
		return 0;
	else if (core == 1)
		return 8;
}

double percentage(double F) {
	F = F * 100.0;
	if (F < 0.0)
		F = 0.0;
	return F;
}

void write_headers() {
	std::ofstream outfile;
	outfile.open(DUMP_FILE);
	outfile << "Date, "
		<< "Time, "
		<< "Total_power, "
		<< "PP0, "
		<< "DRAM, "
		<< "CPU%, "
		<< "Clock_gated%, "
		<< "IDLE%, "
		<< "mem_bdw"
		<< std::endl;
	outfile.close();
}

void write_samples (struct tm *tinfo, power pwr, procstat st, double mem) {
	std::ofstream outfile;
	outfile.open(DUMP_FILE, std::ios::out | std::ios::app);
	outfile << tinfo->tm_year+1900<<"-"<<tinfo->tm_mon<<"-"<<tinfo->tm_mday<<", " << tinfo->tm_hour<<":"<<tinfo->tm_min<<":"<<tinfo->tm_sec<< ", "
		<< pwr.get_total_power() << ", "
		<< pwr.get_pp0_power() << ", "
		<< pwr.get_dram_power() << ", "
		<< st.get_percent_cpu() << ", "
		<< pwr.get_percent_cstate() << ", "
		<< st.get_percent_idle() <<", "
		<< mem
		<< std::endl;
	outfile.close();
}
