#include "cmsis_os2.h"
#include "stm32f4xx.h"

extern void tick(void *arg);
////////////////////////////////////////////////////////////////////////////////////////////
volatile int tickCnt;

volatile int HeatLevelA; //A for stepper
volatile int HeatLevelB; //B for powerSource
volatile float ProccesorTemp;

volatile int GeneralHumdity; //A for stepper
volatile int GeneralTemp; //B for powerSource
////////////////////////////////////////////////////////////////////////////////////////////
#define ADC_COUNT_SAMPLE 10
extern int adc(int ch);
extern float adcTemp(void);


#define GENERAL_TICK_DEALY_MS 2

#define DHT_DELAY 2000/GENERAL_TICK_DEALY_MS  // each 1 sec one sampling
extern int dhtRead(int *temp,int *humdity);


void tick(void *arg){ // tick sampling for avoid multiusage of hardware
	int i=0,i_dh=0,err=0;
	int adc0_sum=0,adc1_sum=0,adc2_sum=0,gpv0,gpv1;
	float temp=0;

	//tick led that show system alive -- level0 debug
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER|=GPIO_MODER_MODE13_0;
	GPIOC->OTYPER&=~(GPIO_OTYPER_OT13);
	GPIOC->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD13_0;
	
	
	HeatLevelA=0;
	HeatLevelB=0;

	ProccesorTemp=0;
	tickCnt=0;
	
	GeneralHumdity=0;
	GeneralTemp=0;
	
	while(1){
		///////////////////////////////////////////////////////
		// Heat Control Sensors of Drivers and power
	  adc0_sum=0;
		adc1_sum=0;
		adc2_sum=0;
	  temp=0;		
		for(int j=0;j<ADC_COUNT_SAMPLE;j++){
			temp+=adcTemp();
			adc0_sum+=adc(0);
			adc1_sum+=adc(1);
		}
		HeatLevelA=adc0_sum/ADC_COUNT_SAMPLE;
		HeatLevelB=adc1_sum/ADC_COUNT_SAMPLE;
		ProccesorTemp=temp/ADC_COUNT_SAMPLE;		
	  /////////////////////////////////////////////////////
		// humadity and general Temputere control
		i_dh++;
		if(i_dh>DHT_DELAY){
			err+=dhtRead(&gpv0,&gpv1);
			i_dh=0;
			GeneralHumdity=gpv1;
			GeneralTemp=gpv0;
		}
		
		//////////////////////////////////////////////////////
		osDelay(GENERAL_TICK_DEALY_MS); //sampling Delay
		///////////////////////////////
		//show that it still work
		if(i==0) GPIOC->BSRR=1<<13;
		if(i==50) GPIOC->BSRR=1<<29;
		i++;
		if(i>100) i=0;
		////////////////////////////////	
		tickCnt++; //new data is ready
		err=0;
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////




