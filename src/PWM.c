
// STM32 4-channel Servo Demo for 168 MHz STM32F4 Discovery - sourcer32@gmail.com

/**************************************************************************************/

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_it.h"

// Integers, or scaled integers (*10) will be more efficient here

volatile uint16_t servo_position[4] = { 0, 1000, 1500, 2000}; // +/- 90.0 degrees

void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    TIM_SetCompare1(TIM4, servo_position[0] ); // PD.12
    TIM_SetCompare2(TIM4, servo_position[1] ); // PD.13
    TIM_SetCompare3(TIM4, servo_position[2] ); // PD.14
    TIM_SetCompare4(TIM4, servo_position[3] ); // PD.15
    }
}

/**************************************************************************************/

void RCC_Configuration(void)
{
  /* --------------------------- System Clocks Configuration -----------------*/
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOD clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
}

/**************************************************************************************/

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM4 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************/

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*-------------------------- GPIO Configuration ----------------------------*/
  /* GPIOD Configuration: Pins 12, 13, 14 and 15 in output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Connect TIM4 pins to AF2 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_10);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF_10);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF_10);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_10);
}

/**************************************************************************************/

void TIM4_Configuration(void)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* Time base configuration - SystemCoreClock = 168000000 for 168 MHz board */
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (((SystemCoreClock / 1000000) / 2) - 1); // Shooting for 1 MHz, (1us)
  TIM_TimeBaseStructure.TIM_Period = 20000 - 1; // 1 MHz / 20000 = 50 Hz (20ms)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Enable TIM4 Preload register on ARR */
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  /* TIM PWM1 Mode configuration: Channel */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1500; // Servo Top-Center
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  /* Output Compare PWM1 Mode configuration: Channel1 PD.12 */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* Output Compare PWM1 Mode configuration: Channel2 PD.13 */
  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* Output Compare PWM1 Mode configuration: Channel3 PD.14 */
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* Output Compare PWM1 Mode configuration: Channel4 PD.15 */
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* TIM Interrupts enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
}

/**************************************************************************************/

void setup_pwm(void)
{
    RCC_Configuration();

    GPIO_Configuration();

    NVIC_Configuration();

    TIM4_Configuration();
}
