#include "stm32f4xx.h"
/*
extern int adc(int ch);
extern float adcTemp(void);
*/

static int IsInit=0;
/////////////////////////////////////////////////////////////////////////////

void adcInit(void){
	RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER|=GPIO_MODER_MODE0|GPIO_MODER_MODE1|GPIO_MODER_MODE2;
	
	ADC->CCR|=ADC_CCR_TSVREFE|ADC_CCR_ADCPRE_1;
	ADC1->CR2=1;
	ADC1->CR1=ADC_CR1_EOCIE;
	IsInit=1;
}





#define V25					(float)2.5 
#define Avg_Slope			(float)0.76	
#define VCC					(float)3.3
#define ADC_RESULATION  (float)4096
float adcTemp(void){
	if(IsInit==0) adcInit();
	ADC1->SR=0;
	ADC->CCR|=ADC_CCR_TSVREFE;
	ADC1->SQR3=18;
	ADC1->CR2|=ADC_CR2_SWSTART;
	while((ADC1->SR&2)==0);
	return ((float)((float)ADC1->DR*VCC/ADC_RESULATION)-V25)/Avg_Slope+(float)25.0;
}

int adc(int ch){
	if(IsInit==0) adcInit();
	ADC1->SR=0;
	ADC1->SQR3=ch&0xf;
	ADC1->CR2|=ADC_CR2_SWSTART;
	while((ADC1->SR&2)==0);
	return (int)ADC1->DR;
}

