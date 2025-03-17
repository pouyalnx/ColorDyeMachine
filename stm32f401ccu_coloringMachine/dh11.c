#include "cmsis_os2.h"
#include <stdio.h>
#include "stm32f401xc.h"
/*
	simple tutorial application of using DHT11 with stm32f401ccu
	
	how USE
	

	extern int dhtRead(int *temp,int *humdity); -->0 === OK
	
	sampling only work in peroid bigger than 1 SEC
*/
extern void dhtInit(void);
extern int dhtRead(int *temp,int *humdity);
///////////////////////////////////////////////////////////////////////////
#define TMR_TIMEOUT_CNT		1000000
#define TMR_PSC				82
#define LEVEL_DETECT 		110
#define LEN_BITS				43

///////////////////////////////////////////////////////////////////////////
static int dht11_data[LEN_BITS];
static volatile int dht11_ptr;
static int is_dht_init=0;

void EXTI0_IRQHandler(void){
	dht11_data[dht11_ptr++]=(int)TIM3->CNT;
	TIM3->CNT=0;
	EXTI->PR|=EXTI_PR_PR0;
	if(dht11_ptr==LEN_BITS){
		TIM3->CR1=0;
		NVIC_DisableIRQ(EXTI0_IRQn);	
	}
}



int dhtRead(int *temp,int *humdity){
	int i;
	int byte,crc_check,crc;
	if(is_dht_init==0) dhtInit();
	
	GPIOA->MODER|=GPIO_MODER_MODE0_0;
	GPIOA->BSRR=GPIO_BSRR_BR0;
	osDelay(20);
	GPIOA->BSRR=GPIO_BSRR_BS0;
	GPIOA->MODER&=~(GPIO_MODER_MODE0_0);
	dht11_ptr=0;
	TIM3->CR1=1;
	TIM3->CNT=0;
	EXTI->PR|=EXTI_PR_PR0;
	NVIC_EnableIRQ(EXTI0_IRQn);	
	while(dht11_ptr!=LEN_BITS && TIM3->CNT<TMR_TIMEOUT_CNT);
	TIM3->CR1=0;
	NVIC_DisableIRQ(EXTI0_IRQn);
	if(TIM3->CNT<TMR_TIMEOUT_CNT){
		for(i=0;i<LEN_BITS;i++)	if(dht11_data[i]>LEVEL_DETECT)	dht11_data[i]=1;
										else										dht11_data[i]=0;
		crc_check=0;
		i=3;
		byte=128*dht11_data[i]+64*dht11_data[i+1]+32*dht11_data[i+2]+16*dht11_data[i+3]+8*dht11_data[i+4]+4*dht11_data[i+5]+2*dht11_data[i+6]+dht11_data[i+7];
		*humdity=byte;
		crc_check+=byte;
		i=11;
		byte=128*dht11_data[i]+64*dht11_data[i+1]+32*dht11_data[i+2]+16*dht11_data[i+3]+8*dht11_data[i+4]+4*dht11_data[i+5]+2*dht11_data[i+6]+dht11_data[i+7];
		crc_check+=byte;
		i=19;
		byte=128*dht11_data[i]+64*dht11_data[i+1]+32*dht11_data[i+2]+16*dht11_data[i+3]+8*dht11_data[i+4]+4*dht11_data[i+5]+2*dht11_data[i+6]+dht11_data[i+7];
		*temp=byte;
		crc_check+=byte;
		i=27;
		byte=128*dht11_data[i]+64*dht11_data[i+1]+32*dht11_data[i+2]+16*dht11_data[i+3]+8*dht11_data[i+4]+4*dht11_data[i+5]+2*dht11_data[i+6]+dht11_data[i+7];
		crc_check+=byte;
		crc_check&=0xff;
		i=35;
		crc=128*dht11_data[i]+64*dht11_data[i+1]+32*dht11_data[i+2]+16*dht11_data[i+3]+8*dht11_data[i+4]+4*dht11_data[i+5]+2*dht11_data[i+6]+dht11_data[i+7];
		if(crc_check==crc)   return 0;		
	}
	return 1;	
}




void dhtInit(void){
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	
	
	GPIOA->MODER&=~(GPIO_MODER_MODE0_0|GPIO_MODER_MODE0_1);
	GPIOA->OTYPER&=~(GPIO_OTYPER_OT0);
	GPIOA->OSPEEDR|=GPIO_OSPEEDER_OSPEEDR0_1;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD0_0;
	

	TIM3->CR1=0;
	TIM3->CNT=0;
	TIM3->PSC=TMR_PSC-1;
	TIM3->EGR|=1;
	
	EXTI->PR|=EXTI_PR_PR0;
	EXTI->FTSR|=EXTI_FTSR_TR0;
	EXTI->IMR|=EXTI_IMR_IM0;
	*(SYSCFG->EXTICR)&=~SYSCFG_EXTICR1_EXTI0_Msk;
	*(SYSCFG->EXTICR)|=SYSCFG_EXTICR1_EXTI0_PA;
	is_dht_init=1;
}
