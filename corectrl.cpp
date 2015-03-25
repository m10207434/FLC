/*
*		corectrl.cpp - by P.Kai
*		
*		To maintain and control CPUs' info 
*
*/


#include "corectrl.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
using namespace std;

string config;
fstream fin;
fstream fout;
char buffer[100];
const int num_freq = 16;
int aval_freq[num_freq]={3401000 ,3400000 ,3300000 ,3100000 ,3000000 ,2900000, 2800000 ,2600000, 2500000, 2400000 ,2200000, 2100000, 2000000 ,1900000 ,1700000 ,1600000};

string i2s(int &i){
	string s;
	stringstream ss(s);
	ss<<i;
	return ss.str();
}

int cpu_get_temp(int cpu){
	int core = cpu+2;
	config = "/sys/module/coretemp/drivers/platform:coretemp/coretemp.0/temp" + i2s(core) + "_input";
	fin.open(config.c_str(),ios::in);
	double t = -1;
	if(fin.is_open()){		
		fin >> buffer;
		t = atof(buffer);
		fin.close();		
	}	
	return t/1000;
}
int cpu_get_freq(int cpu){
	config = "/sys/devices/system/cpu/cpu" + i2s(cpu) + "/cpufreq/cpuinfo_cur_freq";
	fin.open(config.c_str(),ios::in);
	double t = -1;
	if(fin.is_open()){		
		fin >> buffer;
		t = atof(buffer);
		fin.close();		
	}
	return t;
}
void cpu_set_freq(int cpu,int FL){
	if(FL >= num_freq){FL = num_freq - 1;}
	if(FL < 0){FL = 0;}
	config = "echo userspace > /sys/devices/system/cpu/cpu" + i2s(cpu) + "/cpufreq/scaling_governor";
	system(config.c_str());
	config = "echo " + i2s(aval_freq[FL]) + " > /sys/devices/system/cpu/cpu" + i2s(cpu) + "/cpufreq/scaling_setspeed";
	system(config.c_str());	
}


