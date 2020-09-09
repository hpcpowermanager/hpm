#ifndef MSR_H
#define MSR_H

#include <cstdint>

int open_msr(int core);
uint64_t read_msr(int fd, int msr_offset);
void close_msr(int core);


#endif
