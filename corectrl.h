#ifndef	CORETEMP_H
#define	CORETEMP_H

int cpu_get_temp(int cpu);
int cpu_get_freq(int cpu);
int cpu_get_freq(int cpu,int freq);
void cpu_set_freq(int cpu,int FL);

#endif	/*CORETEMP_H*/