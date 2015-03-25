#ifndef	TASK_H
#define	TASK_H

#define second_loop 620000000
#define msecond_loop 620000
#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)


void task_init();

void task_gen(int num,int CPU,int SL,int FL);
void task_stop(int num);

void task_set_SL(int num,int SL);
void task_set_FL(int num,int FL);
void task_set_CPU(int num,int CPU);

double task_get_U(int num);
int task_get_CPU(int num);
int task_get_SR(int num);

#endif	/*TASK_H*/