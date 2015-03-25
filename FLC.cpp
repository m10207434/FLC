/*
*		FLC.cpp - by P.Kai
*		
*		This is the main function of FLC which implement the FLC Algorithm.
*
*/
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <fstream>

#include "corectrl.h"
#include "task.h"
#include "flc.h"
//vvvvvvvvvvvvvv3 

using namespace std;

int FL[] = {0,4,8,12,15};

int main(){
	/*-------------------------------
			Mother setting : 
			CPU : 0(4)
			Freq : MAX FL0
	--------------------------------*/
	cpu_set_t set;
	pid_t pid = getpid();
	CPU_ZERO(&set);
	CPU_SET(0, &set);
	sched_setaffinity(pid, sizeof(cpu_set_t), &set);
	cpu_set_freq(0,0);
	cpu_set_freq(4,0);
	printf("FLC Core:%d\n",pid);
/*	
	cout<<"pause...\n";
	cin.get();
	return 0;
*/
#if FLC == 1
	/*-------------------------------
			Initial Table
	--------------------------------*/
	int FLC_Table[FL_NUM][SR_NUM][Time_NUM];
	ifstream fin("FLC_Table");
	if(fin.is_open()){
		cout<<"Open file secceed!"<<endl;
		char buffer[100];
		for(int FL = 0;FL < FL_NUM;FL++){
			//printf("FL:%d\n",FL);
			fin >> buffer;
			int FL_IN = atoi(buffer);
			if(FL_IN != FL){cout<<"Error : FL not correct"<<endl;exit(0);}
			for(int SR = 1;SR < SR_NUM;SR++){
				//printf("SR:%d\n",SR);
				fin >> buffer;
				int SR_IN = atoi(buffer);
				if(SR_IN != ((SR-1)*5+10)){cout<<"Error : SR not correct"<<endl;exit(0);}
				for(int time = 0;time < Time_NUM;time++){					
					fin >> buffer;
					int TIME_IN = atoi(buffer);
					if(TIME_IN != time){printf("Error : time not correct, FL:%d , SR:%d\n",FL,SR);exit(0);}
					fin >> buffer;
					int TEMP_IN = atoi(buffer);				
					FLC_Table[FL][SR][time] = TEMP_IN;				
				}
				
			}
		}		
		fin.close();
		cout<<"Input table done!"<<endl;
	}else{
		cout<<"Miss file : FLC_Table\n";
		exit(0);
	}	
#if DEBUG==1	
	cout<<"pause...\n";
	cin.get();
	int i = 4;
	int j = 9;
	int k =	0;
	printf("FL:%d\tSR:%d\n",i,j);
	for(;k < Time_NUM;k++){
		printf("%d\t%d\n",k,FLC_Table[i][j][k]);
	}
	return 0;
#endif	
	
#endif	
	ofstream fout;
	ofstream flc_sched;
	char s[100];
	int TimeTick = 0;
	int TimeTickSet = 3000;
	int fileFL = 0;
	int SR = 1;
	int SRSET = 10;
#if FLC == 0
	sprintf(s,"TempTable_record_FLC_FL_%d",fileFL);
	fout.open(s);
	sprintf(s,"Time\tCore1_T\tCore2_T\tCore3_T\tCore1_U\tCore2_U\tCore3_U\n");
	fout << s;	
	while(1){
		while(cpu_get_temp(2) > 45){
			printf("Too hot! T = %d\n",cpu_get_temp(2) );
			sleep(1);
		}
		printf("Start! FL=%d SR=%d\n",fileFL,SR);	
		sprintf(s,"SR=%d\n",SR);
		fout << s;	
		task_gen(1,1,SR,FL[fileFL]);
		cpu_set_freq(1,FL[fileFL]);
		cpu_set_freq(1+4,FL[fileFL]);
		task_gen(2,2,SR,FL[fileFL]);
		cpu_set_freq(2,FL[fileFL]);
		cpu_set_freq(2+4,FL[fileFL]);
		task_gen(3,3,SR,FL[fileFL]);
		cpu_set_freq(3,FL[fileFL]);
		cpu_set_freq(3+4,FL[fileFL]);
		
		while(TimeTick <= TimeTickSet){
			if(TimeTick % 60 == 0){
				printf("Time:%d\tTesting SR:%d , FL:%d\n",TimeTick,SR,fileFL);
			}			
			sprintf(s,"%d\t%d\t%d\t%d\t%f\t%f\t%f\n",
						TimeTick,
						cpu_get_temp(1),cpu_get_temp(2),cpu_get_temp(3),
						task_get_U(1),task_get_U(2),task_get_U(3));
			fout << s;
			TimeTick ++;
			sleep(1);
		}
		
		task_stop(1);
		task_stop(2);
		task_stop(3);
		SR++;
		TimeTick = 0;
		sleep(1);
		
		if(SR >= SRSET){
			SR = 1;
			fileFL ++;
			fout.close();	
		}
		
		if(fileFL >= 5){
			fout.close();
			system("killall FLC");
		}else if(SR == 1){
			sprintf(s,"TempTable_record_FLC_FL_%d",fileFL);
			fout.open(s);
			sprintf(s,"Time\tCore1_T\tCore2_T\tCore3_T\tCore1_U\tCore2_U\tCore3_U\n");
			fout << s;
		}		
	}
#elif FLC == 1
	while(cpu_get_temp(2) > 45){
		printf("Too hot! T = %d\n",cpu_get_temp(2) );
		sleep(1);
	}	
	/*----------------------------
			Task Generating
	-----------------------------*/	
	int Task_SR[] = {0,3,3,3,6,6,6};
	int Task_FL[] = {0,0,0,0,0,0,0};
	
	task_gen(1,1,Task_SR[1],Task_FL[1]);	
	task_gen(2,2,Task_SR[2],Task_FL[2]);
	task_gen(3,3,Task_SR[3],Task_FL[3]);
	task_gen(4,1,Task_SR[4],Task_FL[4]);	
	task_gen(5,2,Task_SR[5],Task_FL[5]);
	task_gen(6,3,Task_SR[6],Task_FL[6]);
	
	core_info Core[Core_NUM];
	sprintf(s,"FLC_System");
	fout.open(s);
	sprintf(s,"Time\tCore1_T\tCore2_T\tCore3_T\tCore1_U\tCore2_U\tCore3_U\n");
	fout << s;
	sprintf(s,"FLC_SCHED");
	flc_sched.open(s);
	sprintf(s,"Time\tHot_Core\tHot_Temp\tFL1\tFL2\tFL3\tFL4\tFL5\tFL6\tT_Set\tT_Preserve\n");
	flc_sched << s;	
	while(1){ //1s record system , 10s FLC	
		sleep(1);
		TimeTick += 1;
		for(int core=0; core < Core_NUM; core++){
			Core[core].tot_u = 0;
		}
		Core[task_get_CPU(1)].tot_u += task_get_U(1);
		Core[task_get_CPU(2)].tot_u += task_get_U(2);
		Core[task_get_CPU(3)].tot_u += task_get_U(3);
		Core[task_get_CPU(4)].tot_u += task_get_U(4);
		Core[task_get_CPU(5)].tot_u += task_get_U(5);
		Core[task_get_CPU(6)].tot_u += task_get_U(6);
		
		//sprintf(s,"Time\tCore1_T\tCore2_T\tCore3_T\tCore1_U\tCore2_U\tCore3_U\n");
		sprintf(s,"%d\t%d\t%d\t%d\t%f\t%f\t%f\n",
						TimeTick,
						cpu_get_temp(1),cpu_get_temp(2),cpu_get_temp(3),
						Core[1].tot_u,Core[2].tot_u,Core[3].tot_u);
		fout << s;
		/*
		printf("-------------------------\n");
		for(int core=0; core < Core_NUM; core++){
			printf("Core%d\tU=%f\n",core,Core[core].tot_u);			
		}
		printf("-------------------------\n");
		*/
		//FLC		
		if(TimeTick % 10 == 0){
			//find the most hot core
			int hot_core = 1;int hot_temp = 0;
			for(int h=1;h<Core_NUM;h++){
				int temp = cpu_get_temp(h);
				if(temp > hot_temp){
					hot_temp = temp;
					hot_core = h;
				}
			}
			//printf("Hot_temp%d\n",hot_temp);
			for(int task = 1; task <=Task_NUM;task++){
				//find the time with the same situation
				int cur_time = 600;
				for(; cur_time >=0; cur_time--){
					if(hot_temp >= FLC_Table[Task_FL[task]][Task_SR[task]][cur_time]){
						break;
					}	
				}
				/*
				printf("Task%d Match FLC_Table[%d][%d][%d] = %d\n",
						task,
						Task_FL[task],Task_SR[task],cur_time,
						FLC_Table[Task_FL[task]][Task_SR[task]][cur_time]);
				*/
				if(cur_time < 0){cur_time=0;}
				if(cur_time > 590){cur_time=590;}
				//find FL
				int set_FL = 0;				
				cur_time += 10;
				for(;set_FL < FL_NUM;set_FL++){
					if(FLC_Table[set_FL][Task_SR[task]][cur_time] < Temp_set - Temp_preserve){
						break;
					}
				}
				/*
				printf("Task%d Match Predict_Table[%d][%d][%d] = %d\n",
						task,
						set_FL,Task_SR[task],cur_time,
						FLC_Table[set_FL][Task_SR[task]][cur_time]);
				*/
				if(set_FL >= FL_NUM){set_FL=FL_NUM - 1;}
				Task_FL[task] = set_FL;				
				//set FL			
				task_set_FL(task,Task_FL[task]);
			}
			//Recording
			//sprintf(s,"Time\tHot_Core\tHot_Temp\tFL1\tFL2\tFL3\tFL4\tFL5\tFL6\tT_Set\tT_Preserve\n");
			sprintf(s,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
						TimeTick,
						hot_core,hot_temp,
						Task_FL[1],Task_FL[2],Task_FL[3],Task_FL[4],Task_FL[5],Task_FL[6],
						Temp_set,Temp_preserve);
			flc_sched << s;	
			printf("-------------------------\n");
			printf("Time:%d\nFL1:%d\nFL2:%d\nFL3:%d\nFL4:%d\nFL5:%d\nFL6:%d\nT_Set:%d\nT_Preserve:%d\n",
					TimeTick,
					Task_FL[1],Task_FL[2],Task_FL[3],Task_FL[4],Task_FL[5],Task_FL[6],
					Temp_set,Temp_preserve);
			printf("-------------------------\n");
		}
		
		//End test
		if(TimeTick >= TimeTickSet){
			fout.close();
			flc_sched.close();
			system("killall FLC");
		}
		
	}
#endif	
	
	
/*	
	//task_gen(int num,int CPU,int SR,int FL)
	task_gen(1,3,5,0);
	for(int sr = 0 ; sr <5 ; sr++){
		task_set_FL(1,FL[sr]);
		sleep(5);
	}
*/	
	
	return 0;
}
