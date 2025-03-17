#include "stm32f4xx.h"
/*
	extern void alarm(int level);
	extern void alarm_tune_mode(int enable);
*/

static int IsInit=0;
static int alarmDelay,alarmCnt;

/////////////////////////////////////////////////////////////////////////////


void TIM5_IRQHandler(void){
	 if(alarmDelay==0) GPIOC->BSRR=1<<30;	
	 else{
		  alarmCnt++;
		  if(alarmCnt>=alarmDelay){
				if(GPIOC->ODR&(1<<14)) GPIOC->BSRR=1<<30;
				else                   GPIOC->BSRR=1<<14;
				alarmCnt=0;
			}
	 }		 
	TIM5->SR=0;	
	
}


////////////////////////////////////////////////////////////////////////////




void alarmInit(){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE14_0;
	GPIOC->OTYPER&=~(GPIO_OTYPER_OT14);
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR14_1;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD14_0;

	RCC->APB1ENR|=RCC_APB1ENR_TIM5EN;
	TIM5->CR1=TIM_CR1_DIR;
	TIM5->DIER|=TIM_DIER_CC1IE;
	TIM5->SR=0;
	TIM5->CCR1=50000;
	TIM5->ARR=50000+1; // each ms 50 tick
	TIM5->PSC=84-1;
	TIM5->EGR=TIM_EGR_UG;
	TIM5->CR1|=TIM_CR1_CEN;

	alarmCnt=0;
	alarmDelay=0;
	
	NVIC_EnableIRQ(TIM5_IRQn);
	IsInit=1;
}

void alarm_tune_mode(int enable){
	if(IsInit==0) alarmInit();
	if(enable==1){
		TIM5->CCR1=100;
		TIM5->ARR=100+1; // each ms 10 tick
		TIM5->CNT=0;
		TIM5->EGR=TIM_EGR_UG;				
	}
	else{
		TIM5->CCR1=50000;
		TIM5->ARR=50000+1; // each ms 50 tick
		TIM5->CNT=0;
		TIM5->EGR=TIM_EGR_UG;		
	}
}


void alarm(int level){ //level 0 is off
	if(IsInit==0) alarmInit();
	if(level>10) level=10;
	if(level<0)  level=0;
	alarmDelay=level;
}

