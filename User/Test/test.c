#include "test.h"
#include "bsp_esp8266.h"
#include "bsp_SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "bsp_dht11.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_motor.h"


/**
  * @brief  ESP8266 ��Sta Tcp Client��͸��
  * @param  ��
  * @retval ��
  */
void ESP8266_StaTcpClient_UnvarnishTest ( void )
{
	uint8_t ucId, ucLen;
	uint8_t ucLed1Status = 0, ucLed2Status = 0, ucLed3Status = 0, ucBuzzerStatus = 0;
	uint8_t motorflag = 0;        //�����־����,0����ֹ/1����ת/2����ת
	uint16_t cnt = 0;
	
	char cStr [ 100 ] = { 0 }, cCh;

  char * pCh, * pCh1;

	DHT11_Data_TypeDef DHT11_Data;
	

  printf ( "\r\n�������� ESP8266 ......\r\n" );

	macESP8266_CH_ENABLE();                                     			 				  //PB8��WiFi_ENʹ��������λ
		
	ESP8266_AT_Test ();                                												 //PB9��WiFi_RST��λ������λ

	ESP8266_Net_Mode_Choose ( AP );                   												 //����ΪAP������㣩ģʽ

  while ( ! ESP8266_CIPAP ( macUser_ESP8266_TcpServer_IP ) );     					//����APģʽ��ip��ַ

  while ( ! ESP8266_BuildAP ( macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd, macUser_ESP8266_BulitApEcn ) );	        //����AP�ȵ�
	
	ESP8266_Enable_MultipleId ( ENABLE );                                     //���������
	
	while ( !	ESP8266_StartOrShutServer ( ENABLE, macUser_ESP8266_TcpServer_Port, macUser_ESP8266_TcpServer_OverTime ) );        //������ģʽ

	ESP8266_Inquire_ApIp ( cStr, 20 );
	printf ( "\r\n��ģ��WIFIΪ%s������Ϊ:%s \r\nAP IP Ϊ��%s�������Ķ˿�Ϊ��%s\r\n�ֻ������������Ӹ� IP �Ͷ˿ڣ���������5���ͻ���\r\n",
           macUser_ESP8266_BulitApSsid, macUser_ESP8266_BulitApPwd , cStr, macUser_ESP8266_TcpServer_Port );
	
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
	
	while ( 1 )
	{		
		if(motorflag == 0)
		{
			cnt = 0;
			MotorStop();
		}
		else if(motorflag == 1) 
		{
			cnt++;
			if(cnt <= 512)
				MotorCW();
			else
			{
				motorflag = 0;
				cnt = 0;
			}
		} 
		else if(motorflag == 2)
		{
			cnt++;
			if(cnt <= 512)
				MotorCCW();
			else
			{
				motorflag = 0;
				cnt = 0;
			}
		}
		
		if( GPIO_ReadInputDataBit(SMOKE_GPIO_PORT,SMOKE_GPIO_PIN) == KEY_OFF )
		{
			while(1) 
			{
				macBEEP_ON ();
				macLED1_ON ();
				Delay_ms(1000);
				macLED1_OFF ();
				macLED2_ON ();
				Delay_ms(1000);
				if( GPIO_ReadInputDataBit(SMOKE_GPIO_PORT,SMOKE_GPIO_PIN) == KEY_ON  )
				{
					macLED1_OFF ();
					macLED2_OFF ();
					macBEEP_OFF ();
					break;
				}
			}
		}
		
		if ( strEsp8266_Fram_Record .InfBit .FramFinishFlag )
		{
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, DISABLE ); //���ô��ڽ����ж�
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';
			
//			printf ( "\r\n%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );//
			if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LED_" ) ) != 0 )                      //����TCP֡��ʽ��"���ƹؼ���"+"ѡ��λ"+0+"����λ"
			{
				cCh = * ( pCh + 8 );
				
				switch ( cCh )
				{
					case '1':
						cCh = * ( pCh + 10 );
					  switch ( cCh )
					  {
							case '0':
								macLED1_OFF ();
								macBEEP_OFF ();
							  ucLed1Status = 0;
						    break;
							case '1':
								macLED1_ON ();
								macBEEP_ON ();
							  ucLed1Status = 1;
						    break;
							default :
								break;
						}
						break;
						
					case '2':
						cCh = * ( pCh + 10 );
					  switch ( cCh )
					  {
							case '0':
								macLED2_OFF ();
							  ucLed2Status = 0;
						    break;
							case '1':
								macLED2_ON ();
							  ucLed2Status = 1;
						    break;
							default :
								break;
						}
						break;

					case '3':
						cCh = * ( pCh + 10 );
					  switch ( cCh )
					  {
							case '0':
								macLED3_OFF ();
							  ucLed3Status = 0;
						    break;
							case '1':
								macLED3_ON ();
							  ucLed3Status = 1;
						    break;
							default :
								break;
						}
						break;
						
				  default :
            break;					
						
				}
				
				sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_END", ucLed1Status, ucLed2Status, ucLed3Status );
				
			}
			
			else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_LIGHT_" ) ) != 0 )          //���տ��Ƶƹ�
			{
				cCh = *(pCh + 10);
				
				switch( cCh )
				{
					case '0' :
						macLED1_OFF ();
						macLED2_OFF ();
						macLED3_OFF ();
						break;
					case '1' :
						macLED1_ON ();
						macLED2_ON ();
						macLED3_ON ();
						break;
					default :
						break;
				}
						sprintf ( cStr, "Light_Changed");

			}
			
			else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "OPEN" ) ) != 0 ) 
			{
				motorflag = 1;
			}
			else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "KEEP" ) ) != 0 ) 
			{
				motorflag = 0;
			}
			else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSE" ) ) != 0 ) 
			{
				motorflag = 2;
			}

			
			else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_BUZZER_" ) ) != 0 ) 												//������
			{
				cCh = * ( pCh + 11 );
				
				switch ( cCh )
				{
					case '0':
						macBEEP_OFF ();
					  ucBuzzerStatus = 0;
						break;
					case '1':
						macBEEP_ON ();
					  ucBuzzerStatus = 1;
						break;
					default:
						break;
				}
				
				sprintf ( cStr, "CMD_BUZZER_%d_ENDBUZZER_END", ucBuzzerStatus );
				
			}
				
			else if ( ( ( pCh  = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_UART_" ) ) != 0 ) &&
				        ( ( pCh1 = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "_ENDUART_END" ) )  != 0 ) ) 
			{
				if ( pCh < pCh1)
				{
					ucLen = pCh1 - pCh + 12;
					memcpy ( cStr, pCh, ucLen );
					cStr [ ucLen ] = '\0';
				}
			}

			else if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CMD_READ_ALL_END" ) != 0 ) 
			{
				DHT11_Read_TempAndHumidity ( & DHT11_Data );
				sprintf ( cStr, "CMD_LED_%d_%d_%d_ENDLED_DHT11_%d.%d_%d.%d_ENDDHT11_BUZZER_%d_ENDBUZZER_END", 
									ucLed1Status, ucLed2Status, ucLed3Status, DHT11_Data .temp_int, 
			            DHT11_Data .temp_deci, DHT11_Data .humi_int, DHT11_Data .humi_deci,
                  ucBuzzerStatus );
			}
			
				
			if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD," ) ) != 0 ) 
			{
				ucId = * ( pCh + strlen ( "+IPD," ) ) - '0';
				ESP8266_SendString ( DISABLE, cStr, strlen ( cStr ), ( ENUM_ID_NO_TypeDef ) ucId );
			}
			
		  strEsp8266_Fram_Record .InfBit .FramLength = 0;
	    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;	
			
			USART_ITConfig ( macESP8266_USARTx, USART_IT_RXNE, ENABLE ); //ʹ�ܴ��ڽ����ж�
			
		}
		
	}
	
		
}


