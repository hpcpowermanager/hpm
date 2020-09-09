#ifndef UTILS_H
#define UTILS_H

#include <power.h>
#include <procstat.h>

int pkg_map(int core);
double percentage(double F);

void write_headers ();
void write_samples(struct tm *tinfo, power pwr, procstat st, double mem);

#endif
