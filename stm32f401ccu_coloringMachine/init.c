//	this part of code designed for init hardware
//	draw init gif of poyalnx coloring dy
//  selftest hardwares and calibrate

#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "GUI.h"
#include "arm_math.h"


extern int dhtRead(int *temp,int *humdity);
extern void alarm(int level);
extern void LCD_UPDATE(void);



void init(void){
	alarm(10);
	osDelay(100);
	alarm(9);
	osDelay(100);
	alarm(0);
	GUI_Init();	
	
}

