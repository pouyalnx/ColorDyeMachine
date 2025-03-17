#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////

extern int 	stepper_isbusy(int ch);
extern int	stepper_move(int ch,int steps);
extern void stepper_mode(int mode); //only can be 1 2 4 8

//not really nessarry
extern void stepper_init(void);
extern void stepper_enable(int ch);
extern void stepper_disable(int ch);


//////////////////////////////////////////////////////////////////////////
#define CS0_SET	GPIOA->BSRR=GPIO_BSRR_BS8	
#define CS0_CLR	GPIOA->BSRR=GPIO_BSRR_BR8	

#define DIR0_SET	GPIOA->BSRR=GPIO_BSRR_BS10
#define DIR0_CLR	GPIOA->BSRR=GPIO_BSRR_BR10	

#define STP0_SET	GPIOA->BSRR=GPIO_BSRR_BS9
#define STP0_CLR	GPIOA->BSRR=GPIO_BSRR_BR9


#define M00_SET	GPIOA->BSRR=GPIO_BSRR_BS11
#define M00_CLR	GPIOA->BSRR=GPIO_BSRR_BR11	

#define M01_SET	GPIOA->BSRR=GPIO_BSRR_BS12
#define M01_CLR	GPIOA->BSRR=GPIO_BSRR_BR12


//#define TICK_DELAY_MIN		500
//#define TICK_DELAY_MAX		4000
//////////////////////////////////////////////////////////////////////////
static int ch0_ticks;
static int isInited=0;

void TIM2_IRQHandler(void){
	TIM2->SR=0;
	if(ch0_ticks){
		if(ch0_ticks&1) STP0_CLR;
		else				 STP0_SET;
		ch0_ticks--;
	}
}
//////////////////////////////////////////////////////////////////////////
void stepper_init(void){
	
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER|=GPIO_MODER_MODE8_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT8);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR8_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD8_0;	
	
	GPIOA->MODER|=GPIO_MODER_MODE9_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT9);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR9_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD9_0;	
	
	GPIOA->MODER|=GPIO_MODER_MODE10_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT10);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR10_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD10_0;

	GPIOA->MODER|=GPIO_MODER_MODE11_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT11);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR11_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD11_0;	

	GPIOA->MODER|=GPIO_MODER_MODE12_0;
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT12);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR12_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD12_0;

	M00_CLR;
	M01_CLR;
	CS0_CLR;
	DIR0_CLR;
	STP0_CLR;
	
	ch0_ticks=0;
	
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	TIM2->CR1=TIM_CR1_DIR;
	TIM2->PSC=84-1;
	TIM2->ARR=1000-1;
	TIM2->EGR=TIM_EGR_UG;
	TIM2->DIER=TIM_DIER_UIE;
	TIM2->SR=0;
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->CR1|=TIM_CR1_CEN;
	isInited=1;
}


int stepper_isbusy(int ch){
	if(isInited==0) stepper_init();
	if(ch==0){
		if(ch0_ticks) return 1;
		return 0;
	}
	return -1;
}

int stepper_move(int ch,int steps){
	if(isInited==0) stepper_init();
	if(stepper_isbusy(ch)) return stepper_isbusy(ch);
	if(ch==0){
		CS0_CLR;
		//if(tick_delay_us<TICK_DELAY_MIN) tick_delay_us=TICK_DELAY_MIN;
		//if(tick_delay_us>TICK_DELAY_MAX) tick_delay_us=TICK_DELAY_MAX;
		//TIM2->CNT=0;
		//TIM2->ARR=tick_delay_us-1;
		//TIM2->EGR=TIM_EGR_UG;		
		if(steps<0){ steps=-steps; DIR0_CLR;}
		else{                      DIR0_SET;}
		ch0_ticks=steps*2;
		return 1;
	}
	return -1;
}


void stepper_enable(int ch){
	if(isInited==0) stepper_init();
	if(ch==0) CS0_CLR;
}
void stepper_disable(int ch){
	if(isInited==0) stepper_init();
	if(ch==0) CS0_SET;
}

//as here we dont need mode 16 so i dont use it
void stepper_mode(int mode){ //only can be 1 2 4 8
	if(mode<=1){
		M00_CLR;
		M01_CLR;
	}
	else if(mode==2){
		M00_SET;
		M01_CLR;
	}		
	else if(mode<=4){
		M00_CLR;
		M01_SET;
	}	
	else{
		M00_SET;
		M01_SET;		
	}
}
