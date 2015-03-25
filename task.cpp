/*
*		task.cpp - by P.Kai
*		
*		Task manager 
*		There are 6 tasks for experiment
*/
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <time.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "task.h"
#include "corectrl.h"
using namespace std;

const double u_com = 1.05; 	//compensation of task's sleep time 
bool need_init = true;

typedef struct task_arg{
	key_t mem;
	int FL ;
	int SR ;
	int CPU ;
	int EXE;
	double util;
	pid_t PID ;
}	task_arg;	
	
void  task1 (void * arg){
	cout<<"Task1 Born!\n";
	task_arg *para = (task_arg *)arg;		
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	/*
	int SR[] = {9999999 , 1600000 , 1066667 , 800000 , 640000 , 533333 , 
				457142 , 400000 , 355555 , 320000 , 290909 ,
				266666 , 246153 , 228571 , 213333 , 200000 ,
				188235 , 177777 , 168421 };*/
	int FL[] = {0,4,8,12,15};			
	int SR[] = {9999999 , 1600000 , 1066667 , 800000 , 640000 , 533333 , 
				457142 , 400000 , 355555 , 320000 , 290909 ,
				266666 , 246153 , 228571 , 213333 , 200000 ,
				188235 , 177777 , 168421 };			
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}
//	para->SR = 0;
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);	
	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);	
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		long num_steps = 100000;
		double x, sum=0;
		double step = 1.0 / (double) num_steps;
		for(int i=0 ; i<num_steps ; i++) {		
			for(int j=0 ; j<1000 ; j++);
			x = (i+0.5)*step;
			sum = sum + 4.0/(1.0+x*x);			
		}
		double pi = step*sum;
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 1("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<endl;
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
//		cout<<"Task : "<<&para->CPU<<" = "<<para->CPU<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);
	}
	cout<<"Task1 Dead!\n";
}

void  task2 (void * arg){
	cout<<"Task2 Born!\n";
	task_arg *para = (task_arg *)arg;	
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	int FL[] = {0,4,8,12,15};
	int SR[] = {3600000 , 3600000 , 2400000 , 1800000 , 1440000 , 1200000 ,
				1029000 , 900000 , 800000 , 720000 , 655000 ,
				600000 , 554000 , 514000 , 480000 , 450000 ,
				423500 , 400000 , 379000 };
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}
	//	para->SR = 0;
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);

	
	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);	
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		const int size = 100;
		int MatrixA[size][size];
		int MatrixB[size][size];
		for(int i=0;i<size;i++){	
			for(int j=0;j<size;j++){
				MatrixA[i][j] = rand()%100;
				MatrixB[i][j] = rand()%100;
			}
		}
		for (int i=0; i < 100; i++){
			int MatrixC[size][size]={0};
			for(int i=0;i<size;i++)
			{	
				for(int j=0;j<size;j++)
				{
					MatrixC[i][j]=0;
					for(int k=0;k<size;k++)
					{
						MatrixC[i][j] = MatrixA[i][k] * MatrixB[k][j];
					}
				}
			}			
		}
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 2("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<"\n";
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);		
	}
	cout<<"Task2 Dead!\n";
}

void  task3 (void * arg){
	cout<<"Task3 Born!\n";
	task_arg *para = (task_arg *)arg;	
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	
//	int SR[] = {30333333,18200000,11375000,7583333,6066666};

	int FL[] = {0,4,8,12,15};
	int SR[] = {9999999 , 5600000 , 3733333 , 2800000 , 2240000 , 1866666 ,
				1600000 , 1400000 , 1244444 , 1120000 , 1018182 ,
				933333 , 861538 , 800000 , 746666 , 700000 ,
				658823 , 622222 , 589473};
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}			
//	para->SR = 0;
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);

	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);	
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		int size = 2500;
		double pi = 2.0 * 3.1415926;
		double angleTerm,cosineA,sineA;
		double invs = 1.0/size;
		double real[size], imag[size];
		for(int y = 0 ; y < size ; y++){
			real[y] = 0 ; imag[y] = 0 ;
			for(int x = 0 ; x < size ; x++){
				angleTerm = pi * y * x * invs;
				cosineA = cos(angleTerm);
				sineA = sin(angleTerm);
				int i_r = rand()%1000/10;
				int i_i = rand()%1000/10;
				real[y] += i_r * cosineA - i_i * sineA; 
				imag[y] += i_r * sineA + i_i * cosineA; 
			}
			real[y] *= invs ; imag[y] *= invs;
		}
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 3("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<"\n";
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);
	}
	cout<<"Task3 Dead!\n";
}

void  task4 (void * arg){
	cout<<"Task4 Born!\n";
	task_arg *para = (task_arg *)arg;		
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	int FL[] = {0,4,8,12,15};
	int SR[] = {9999999 , 600000 , 400000 , 300000 , 240000 , 200000 ,
						171429 , 150000 , 133333 , 120000 , 109091 , 
						100000 , 92308 , 85714 , 80000 , 75000 , 
						70588 , 66667 , 63158};
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);	
	
	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);	
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		for(int sleep_t = 0; sleep_t <= msecond_loop*50 ; sleep_t++){}
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 4("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<endl;
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
//		cout<<"Task : "<<&para->CPU<<" = "<<para->CPU<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);
	}
	cout<<"Task4 Dead!\n";
}

void  task5 (void * arg){
	cout<<"Task5 Born!\n";
	task_arg *para = (task_arg *)arg;	
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	int FL[] = {0,4,8,12,15};
	int SR[] = {9999999 , 600000 , 400000 , 300000 , 240000 , 200000 ,
						171429 , 150000 , 133333 , 120000 , 109091 , 
						100000 , 92308 , 85714 , 80000 , 75000 , 
						70588 , 66667 , 63158};
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);

	
	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);	
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		for(int sleep_t = 0; sleep_t <= msecond_loop*50 ; sleep_t++){}
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 5("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<"\n";
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);		
	}
	cout<<"Task5 Dead!\n";
}

void  task6 (void * arg){
	cout<<"Task6 Born!\n";
	task_arg *para = (task_arg *)arg;	
	int shmid = shmget(para->mem , sizeof(struct task_arg) , 0666);
	para = (task_arg *)shmat(shmid , NULL , 0);
	int FL[] = {0,4,8,12,15};
//	int SR[] = {30333333,18200000,11375000,7583333,6066666};
	int SR[] = {9999999 , 3800000 , 2533333 , 1900000 , 1520000 , 1266667 ,
						1085714 , 950000 , 844444 , 760000 , 690909 , 
						633333 , 584615 , 542857 , 506667 , 475000 , 
						447059 , 422222 , 400000};
	for(int i = 0 ; i<19 ; i++){
		SR[i]*=u_com;
	}
	para->PID = getpid();
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(para->CPU, &set);
	sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);

	while(para->SR >= 0){
		cpu_set_freq(para->CPU%4,FL[para->FL]);
		cpu_set_freq(para->CPU%4+4,FL[para->FL]);		
		clock_t start = clock();
		double temp = cpu_get_temp(para->CPU);
		//*************************main***********************
		for(int sleep_t = 0; sleep_t <= msecond_loop*370 ; sleep_t++){}
		//*************************main***********************
		double deltaT = cpu_get_temp(para->CPU);
		para->EXE = (double)(clock()-start)/CLOCKS_PER_MS;
		para->util = (double)para->EXE/(SR[para->SR]/1000);
//		cout<<"Task 6("<<para->PID<<") Done CPU"<<para->CPU<<". EXE = "<<(double)(clock()-start)/CLOCKS_PER_MS<<"ms u = "<<para->util<<"\n";
//		cout<<"T = "<<temp<<" - "<<deltaT<<"\n";
		usleep((SR[para->SR]-para->EXE*1000)<0?0:(SR[para->SR]-para->EXE*1000));
		CPU_ZERO(&set);
		CPU_SET(para->CPU, &set);
		sched_setaffinity(para->PID, sizeof(cpu_set_t), &set);
	}
	cout<<"Task6 Dead!\n";
}


//manager

pid_t task1_p,task2_p,task3_p,task4_p,task5_p,task6_p;
task_arg *task1_arg, *task2_arg, *task3_arg, *task4_arg, *task5_arg, *task6_arg;
key_t task1_key,task2_key,task3_key,task4_key,task5_key,task6_key;	



void task_init(){
	if(need_init){
		//shared memory	
		task1_arg = (task_arg* ) shmat (shmget(task1_key = 1234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);
		task2_arg = (task_arg* ) shmat (shmget(task2_key = 2234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);
		task3_arg = (task_arg* ) shmat (shmget(task3_key = 3234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);	
		task4_arg = (task_arg* ) shmat (shmget(task4_key = 4234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);
		task5_arg = (task_arg* ) shmat (shmget(task5_key = 5234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);
		task6_arg = (task_arg* ) shmat (shmget(task6_key = 6234 , sizeof(struct task_arg) , IPC_CREAT | 0666) , NULL , 0);		
		task1_arg->mem = task1_key;
		task2_arg->mem = task2_key;
		task3_arg->mem = task3_key;
		task4_arg->mem = task4_key;
		task5_arg->mem = task5_key;
		task6_arg->mem = task6_key;
	}
	need_init = false;	
}


void task_gen(int num,int CPU,int SL,int FL){		
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	void (*TaskF[])(void*) = {task1 , task2 , task3 , task4 , task5 , task6};	
	task_set_CPU(num,CPU);	
	task_set_FL(num,FL);
	task_set_SL(num,SL);
	if( fork() == 0){
		TaskF[num-1](TaskQ[num-1]);
		exit(0);
	}	
}
void task_stop(int num){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	TaskQ[num-1]->SR 	= 	-1;
}
void task_set_FL(int num,int FL){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	TaskQ[num-1]->FL = 	FL;	
}
void task_set_CPU(int num,int CPU){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	TaskQ[num-1]->CPU = 	CPU;	
}
void task_set_SL(int num,int SL){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	TaskQ[num-1]->SR = SL;	
}

double task_get_U(int num){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	return TaskQ[num-1]->util;
}

int task_get_CPU(int num){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	return TaskQ[num-1]->CPU;
}

int task_get_SR(int num){
	task_init();
	task_arg* TaskQ[] = {task1_arg , task2_arg , task3_arg , task4_arg , task5_arg , task6_arg};
	return TaskQ[num-1]->SR;
}