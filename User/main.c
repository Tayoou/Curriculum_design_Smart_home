#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main ( void )
{	
	/* ��ʼ�� */
  USARTx_Config ();                                                              //��ʼ������1
	SysTick_Init ();                                                               //���� SysTick Ϊ 1ms �ж�һ�� 
	ESP8266_Init ();                                                               //��ʼ��WiFiģ��ʹ�õĽӿں�����
	DHT11_Init ();
  LED_Init ();
	Beep_Init ();
	Key_GPIO_Config();


	macBEEP_ON ();
	Delay_ms(100);
	macBEEP_OFF ();
	
  ESP8266_StaTcpClient_UnvarnishTest ();
	
	
  while ( 1 );
	
	
}


/*********************************************END OF FILE**********************/
