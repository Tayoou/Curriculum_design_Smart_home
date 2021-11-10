#include "bsp_motor.h"

unsigned char phasecw[8] = {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09}; //正转 电机导通相序  D-DC-C-CB-B-BA-A-AD
unsigned char phaseccw[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09}; //反转 电机导通相序  A-AB-B-BC-C-CD-D-DA

void MotorCW( void )
{
    unsigned char i;
    unsigned char temp = 0;
    for( i = 0; i < 8; i++ )
    {
        temp = phasecw[i];
        LD = ( temp >> 3 ) & 0x01;							//取bit4的值
        LC = ( temp >> 2 ) & 0x01;
        LB = ( temp >> 1 ) & 0x01;
        LA = ( temp >> 0 ) & 0x01;							//取bit0的值
        Delay_ms( 2 ); 										
    }
		MotorStop();
}

void MotorCCW( void )
{
    unsigned char i;
    unsigned char temp = 0;
    for( i = 0; i < 8; i++ )
    {
        temp = phaseccw[i];
        LD = ( temp >> 3 ) & 0x01;							//取bit4的值
        LC = ( temp >> 2 ) & 0x01;
        LB = ( temp >> 1 ) & 0x01;
        LA = ( temp >> 0 ) & 0x01;							//取bit0的值
        Delay_ms( 2 ); 										
    }
		MotorStop();
}

void MotorStop( void )
{
    LA = 0;
    LB = 0;
    LC = 0;
    LD = 0;
}
