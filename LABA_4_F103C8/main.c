#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "stm32f10x_usart.h"
#include <string.h>

volatile char buffer[100] = {'\0'};
int adc_value;
int tiktak=0;
int r=0;


void Delay(u32 ticks)
{
  while(ticks != 0) ticks--;
}


void SetSysClockTo72(void)
{
	ErrorStatus HSEStartUpStatus;
    RCC_DeInit();
    RCC_HSEConfig( RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if (HSEStartUpStatus == SUCCESS)
    {
        RCC_HCLKConfig( RCC_SYSCLK_Div1);
        RCC_PCLK2Config( RCC_HCLK_Div1);
        RCC_PCLK1Config( RCC_HCLK_Div2);
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        RCC_PLLCmd( ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08){}
    }
    else{while (1){}}
}


void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	  switch(tiktak){
	  case 0:
		   GPIO_SetBits(GPIOA, GPIO_Pin_5);
		   GPIO_SetBits(GPIOA, GPIO_Pin_2);
		   GPIO_SetBits(GPIOA, GPIO_Pin_3);
		   Delay(500000);
		   GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		   Delay(500000);
		   GPIO_SetBits(GPIOA, GPIO_Pin_4);
		   Delay(500000);
		   GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		   Delay(500000);
		   r=0;
	  break;
	  case 1:
		  GPIO_SetBits(GPIOA, GPIO_Pin_3);
		  GPIO_SetBits(GPIOA, GPIO_Pin_4);
		  GPIO_SetBits(GPIOA, GPIO_Pin_5);
		  Delay(500000);
		  GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		  Delay(500000);
		  GPIO_SetBits(GPIOA, GPIO_Pin_2);
		  Delay(500000);
		  GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		  Delay(500000);
		  r=1;
	  break;
	  }
	  adc_value = ADC_GetConversionValue(ADC1);


		if(adc_value >= 2126){
    tiktak=1;
		}
		else {
			tiktak=0;
		}
}


void usart_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void USARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    while(ulCount--)
    {
        USART_SendData(USART1, *pucBuffer++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
    }
}

int main(void)
{
  SetSysClockTo72();

  GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);


  ADC_InitTypeDef ADC_InitStructure;
      GPIO_InitTypeDef  GPIO_InitStructure;
      GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
      GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 ;
      GPIO_Init(GPIOA, &GPIO_InitStructure);

      RCC_ADCCLKConfig (RCC_PCLK2_Div6);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);


      ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
      ADC_InitStructure.ADC_ScanConvMode = DISABLE;
      ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
      ADC_InitStructure.ADC_NbrOfChannel = 1;

      ADC_RegularChannelConfig(ADC1,ADC_Channel_1, 1,ADC_SampleTime_28Cycles5);
      ADC_Init ( ADC1, &ADC_InitStructure);

      ADC_Cmd (ADC1,ENABLE);


      ADC_ResetCalibration(ADC1);
      while(ADC_GetResetCalibrationStatus(ADC1));
      ADC_StartCalibration(ADC1);
      while(ADC_GetCalibrationStatus(ADC1));

      ADC_Cmd (ADC1,ENABLE);
      ADC_SoftwareStartConvCmd(ADC1, ENABLE);


      TIM_TimeBaseInitTypeDef TIMER_InitStructure;
      NVIC_InitTypeDef NVIC_InitStructure;

      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
      TIM_TimeBaseStructInit(&TIMER_InitStructure);
      TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
      TIMER_InitStructure.TIM_Prescaler = 7200;
      TIMER_InitStructure.TIM_Period = 4000;
      TIM_TimeBaseInit(TIM4, &TIMER_InitStructure);
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
      TIM_Cmd(TIM4, ENABLE);

      NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);

      usart_init();

  while (1)
  {
	  if (r == 0){
	  	 sprintf(buffer, "\r\nVD2 and VD5 - light VD3 and VD4 - blink\r\n \r\n");
	  	 USARTSend(buffer, sizeof(buffer));
	  	  }
	  if (r == 1){
	  	 sprintf(buffer, "\r\nVD3 and VD4 - light VD2 and VD5 - blink\r\n \r\n");
	  	USARTSend(buffer, sizeof(buffer));
	  	  }
  }
}

