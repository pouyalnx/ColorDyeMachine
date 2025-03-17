#include "cmsis_os2.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "GUI.h"
#include "arm_math.h"

extern void LCD_UPDATE(void);

extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);
extern void stepper_mode(int mode); //only can be 1 2 4 8



extern void alarm(int level);
extern void alarm_tune_mode(int enable);
///////////////////////////////////////////////////////////////////////////////////////////
#define BACK_COLOR		0xD3A9F4
#define SHAPE_COLOR   0x421342

static void on_gui(void){
	GUI_Init();
  GUI_SetBkColor(BACK_COLOR);
	GUI_Clear();	
	GUI_SetColor(GUI_BLUE_98);
	GUI_DrawCircle(60,60,50);
	GUI_DrawCircle(65,55,50);
	GUI_DrawCircle(55,65,50);
	
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_13B_1);
	GUI_DispStringAt("RozhTex",30,25);	
	GUI_SetFont(GUI_FONT_13B_1);
	GUI_DispStringAt("since 2020",20,37);
	GUI_DrawCircle(60,60,50);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_20B_1);
	GUI_DispStringAt("PoyaLnx",20,50);
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16B_1);
	GUI_DispStringAt("DyingMachine",30,70);	
	GUI_SetFont(GUI_FONT_13B_1);
	GUI_DispStringAt("@poyalnx",40,87);		
	
	GUI_SetFont(GUI_FONT_10_1);
	GUI_DispStringAt("V1.0 2024.",70,115);			
	GUI_Exec();
	LCD_UPDATE();		
	
	//alarm_tune_mode(1);
	alarm(1);
	osDelay(5000);
	alarm(0);
	//alarm_tune_mode(0);
	
	
}
///////////////////////////////////////////////////////////////////////////////////////////
extern void tick(void *arg);






extern void alarm(int level);
extern int dhtRead(int *temp,int *humdity);
extern void dye_mode_stop_slow_fast_60min(void);

void app(void *arg){
	//osThreadNew(tick,NULL,NULL);
//	osThreadNew(errorControl,NULL,NULL);
	osDelay(100);
	on_gui();

//	printf("Stepper. D27<\n");
	while(1){
		dye_mode_stop_slow_fast_60min();
	}	
}








