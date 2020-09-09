# HPM
HPM is a power management tool for HPC applications. It currently measures package power, core power, DRAM power, cpu utilization, clock gated residency, idle residency and the DRAM access bandwidth of a given workload with the sampling frequency of one second. The output is generated in the form of a csv file. One can generate the output in the form of graphs or tables using these csv values.

### Build
```
make
```
### Run
```
./hpm ./<your_workload_executable>
```
### Output
The tool generates a dump.csv file as an output that contains the following fields
* “Timestamp” - Timestamp
* “Total power” - RAPL Package Power (In Watts)
* “PPO” - RAPL Core Power (In Watts)
* “DRAM” - RAPL DRAM Power (In Watts)
* “CPU%” - CPU percentage utilization
* “Clock gated%” - Clock gated residency in %
* “IDLE%” - Idle residency in
* “mem bdw” - DRAM access bandwidth in MB/s

One can plot graphs with the csv output data.
