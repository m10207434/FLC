#ifndef	FLC_H
#define	FLC_H


#define FLC 1 	// 0 for building table
				// 1 for FLC
				
#define DEBUG 0 // 0 
				// 1 for showing debug message
				
//Table parameter
#define SR_NUM		10
#define FL_NUM		5
#define Time_NUM	601

//System parameter
#define Core_NUM		4
#define Task_NUM		6
#define Temp_set		80
#define Temp_preserve	10

typedef struct core_info{
	int id;
	double tot_u;
	double avg_temp;
}	core_info;
				

#endif	/*FLC_H*/