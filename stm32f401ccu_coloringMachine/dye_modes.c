#include "cmsis_os2.h"

//////////////////////////////////////////////////////////////////////////////
extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);
extern void stepper_mode(int mode); //only can be 1 2 4 8
extern void stepper_enable(int ch);
extern void stepper_disable(int ch);
//////////////////////////////////////////////////////////////////////////////

extern void dye_mode_stop_slow_fast_60min(void);


//////////////////////////////////////////////////////////////////////////////
#define BASE_DELAY	10
#define SLOW_DURATION	5*60*1000/BASE_DELAY
#define STOP_DURATION	5*60*1000/BASE_DELAY
#define FAST_DURATION	5*60*1000/BASE_DELAY

void dye_mode_stop_slow_fast_60min(void){
	while(stepper_isbusy(0)==1) osDelay(20);
	// before this motor should be free
	for(int i=0;i<4;i++){
		stepper_enable(0);
		stepper_mode(2);
		for(int j=0;j<SLOW_DURATION;j++){
			stepper_move(0,BASE_DELAY*100);
			while(stepper_isbusy(0)==1) osDelay(BASE_DELAY);
		}
		stepper_mode(1);
		for(int j=0;j<FAST_DURATION;j++){
			stepper_move(0,BASE_DELAY*100);
			while(stepper_isbusy(0)==1) osDelay(BASE_DELAY);
		}		
		stepper_disable(0);
		for(int j=0;j<STOP_DURATION;j++){
			osDelay(BASE_DELAY);
		}				
	}
}


