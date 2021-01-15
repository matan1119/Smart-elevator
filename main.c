#include "REG_89C51.h"							 
#include "type_var_H.h"
#include "lcd_play.h"
#include "math.h"

#define VT P3_3	
#define RF_PORT P0

//ISD4004 defines

#define LOW 0
#define HIGH 1
#define TRUE 1
#define FALSE 0
#define SET_PLAY 0xE0
#define SET_RECORD 0xA0
#define SET_MC 0xE8

//Max7219 defines

#define  ZERO      0 
#define  MSB       0x80
#define  LSB       0x01
#define  DECODE_MODE   0x09 
#define  INTENSITY     0x0A 
#define  SCAN_LIMIT    0x0B 
#define  SHUT_DOWN     0x0C 
#define  DISPLAY_TEST  0x0F 

//ISD4004 control bits
sbit    SS = P3^4;
sbit    MOSI = P3^5;
sbit    SCLK = P3^6;
sbit    INTR  = P3^7;


//MAX7219 control bits
sbit LOAD=P1^3; //MAX7219    Load-Data Input:    rising edge  pin 12 
sbit DIN=P1^1; //MAX7219    Serial-Data Input:   rising edge  pin 1
sbit CLK=P1^2; //MAX7219   Serial-Clock Input:  maximum 10MHz  pin 13

 
code u8 welcome_menu[16]= {"    WELCOME     "};
code u8 menu[16]=         {"FLOOR NUMBER:   "};
code u8 floor_menu[16]=   {"GO TO FLOOR     "};
u8 datarf=0,old_floor=1, new_floor=1,i;


code u8 Keys[16] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
bit keyflag;
int keypressed;


void KEY() interrupt 0 using 1
{
//   OE=0;	
   EX0=0;
   keypressed= Keys[(P0&0XF0)>>4];
   keyflag = 1;
//   OE=1;
   EX0=1;

}
 


void Write_Max7219_byte(unsigned char temp)
{
 unsigned char i;
 for (i=0;i<8;i++)     
  { 
   CLK=LOW;
     DIN=(bit)(temp&MSB);      
     temp<<=1;  
     CLK=HIGH;
  }
}


void Write_Max7219(unsigned char address,unsigned char dat)
{ 
 LOAD=LOW;
   Write_Max7219_byte(address); 
   Write_Max7219_byte(dat);
  LOAD=HIGH;                 
}


void Init_Max7219(void)      
{ 
 Write_Max7219(SHUT_DOWN, 0x01);   //Normal Operation XXXXXXX1 Shutdown Mode   XXXXXXXX0
 Write_Max7219(DISPLAY_TEST, 0x00);   //Normal Operation XXXXXXX0 Display Test Mode XXXXXXXX1
 Write_Max7219(DECODE_MODE, 0xff);   //Decode Mode Select D7~D0 1 B decode 0 No decode 
 Write_Max7219(SCAN_LIMIT, 0x03);   //SCAN LIMIT 0~7 0xX0~0xX7 0x07
 Write_Max7219(INTENSITY, 0x02);   //Set Intensity   0xX0~0xXf
}



void nop()
{
 unsigned char i;
 for(i=0;i<2;i++);
}

static void StartISD (void)
{
    SS = HIGH;	//SS - Slave Select (Enable)
    nop();
    SCLK = LOW;
    nop();
    SS = LOW;
    nop();
}

static void StopISD (void)
{
   SCLK    = LOW;
    SS = HIGH;
    nop();
    nop();
}

static void SendByte (unsigned char byte)
{
    unsigned char i = 8;

    SCLK = LOW;
    byte >>= 1;
    do
    {
        MOSI = CY;
        SCLK = HIGH;
        byte >>= 1;
        SCLK = LOW;
    }
    while(--i);
}
static void SendWord (unsigned int word)
{
    unsigned char i = 16;

    SCLK = LOW;
    word >>= 1;
    do
    {
        MOSI = CY;
        SCLK = HIGH;
        word >>= 1;
        SCLK = LOW;
    }
    while(--i);
}
void PowerUp (void)
{
    StartISD();
    SendByte(0x20);
    StopISD();
}

void StopAction(void) //reentrant
{
    StartISD();
    SendByte(0x30);
    StopISD();
}

void PowerDown (void)
{
    StartISD();
    SendByte(0x10);
    StopISD();
}    

void SetPlay (unsigned int address)
{
    StartISD();
    SendWord(address);
    SendByte(SET_PLAY);
    StopISD();
}

void Play (void)
{
    StartISD();
    SendByte(0xF0);
    StopISD();
}

void SetRecord (unsigned int address)
{
    StartISD();
    SendWord(address);
    SendByte(SET_RECORD);
    StopISD();
}

void Record (void)
{
    StartISD();
    SendByte(0xB0);
    StopISD();
}

void DelayIsd (unsigned int x)
{
 unsigned long i;
 for(i=0;i<x*30;i++);
}

void play_track(int addr)
{
	DelayIsd(100);
	PowerUp ();
	DelayIsd(100);
	SetPlay(addr);
	Play();
	while(INTR);
	StopAction();
	DelayIsd(100);

}

void delay(u16 n)
{
	u16 i,m;

	for(i=0;i<n;i++)
		for(m=0;m<100;m++);
}

void elivator()
{
	while(new_floor>old_floor)
	{
		P1_4=1;
		P1_5=0;

		if(new_floor==2)
		{
			while(P1_7==1);
			P1_4=0;
			P1_5=0;
			old_floor=2;
			Write_Max7219(1,old_floor);
			Write_Max7219(2,0);
			Write_Max7219(3,0);

			play_track(0x100);

		}
		else if(new_floor==3)
		{
			while(P2_7==1);
			P1_4=0;
			P1_5=0;
			old_floor=3;
			Write_Max7219(1,old_floor);
			Write_Max7219(2,0);
			Write_Max7219(3,0);

			play_track(0x200);
		}

	}
	while(new_floor<old_floor)
	{
		P1_4=0;
		P1_5=1;
		if(new_floor==2)
		{
			while(P1_7==1);
			P1_4=0;
			P1_5=0;
			old_floor=2;
			Write_Max7219(1,old_floor);
			Write_Max7219(2,0);
			Write_Max7219(3,0);

			play_track(0x100);
		}
		else if(new_floor==1)
		{
			while(P1_6==1);
			P1_4=0;
			P1_5=0;
			old_floor=1;
			Write_Max7219(1,old_floor);
			Write_Max7219(2,0);
			Write_Max7219(3,0);
			play_track(0x00);
		}

	}

}
void main()
{	
													
	lcd_setup();
	delay_lcd();
	lcd_clear();
	delay_lcd();

	Init_Max7219();

	VT=1;
	old_floor=4;
	while(old_floor==4)
	{
		P1_4=0;
		P1_5=1;

		if(P1_6==0)
			old_floor=1;
		else if(P1_7==0)
			old_floor=2;
		else if(P2_7==0)
			old_floor=3;
	 }
	P1_4=0;
	P1_5=0;
	
	Write_Max7219(1,old_floor);
	Write_Max7219(2,0);
	Write_Max7219(3,0);

	GoToCursor(0x80);
	for(i=0;i<16;i++)
		lcd_write(welcome_menu[i]);
	GoToCursor(0xC0);
	for(i=0;i<14;i++)
		lcd_write(menu[i]);
   lcd_write(old_floor+'0');

	new_floor=1;
	RF_PORT=0xFF;

	while(1)
	{
		if(VT==1)
		{
			datarf=RF_PORT&0xf0;
			datarf=datarf>>4;
			if(datarf==14)
				datarf=1;
			else if(datarf==13)
				datarf=2;
			else if(datarf==11)
				datarf=3;
			else if(datarf==7)
				datarf=4;

			switch (datarf)
			{
				case 1: 
				{
					GoToCursor(0xC0);
					for(i=0;i<15;i++)
						lcd_write(floor_menu[i]);
					lcd_write(datarf+0x30);
					new_floor = 1;
					elivator();
				break;
				}
				case 2: 
				{
					GoToCursor(0xC0);
					for(i=0;i<15;i++)
						lcd_write(floor_menu[i]);
					lcd_write(datarf+0x30);
					new_floor = 2;
					elivator();
				break;
				}
				case 3:
				{
					GoToCursor(0xC0);
					for(i=0;i<15;i++)
						lcd_write(floor_menu[i]);
					lcd_write(datarf+0x30);
					new_floor = 3;
					elivator();
				break;
				}
			}
			delay(1000);
		}

		
	}
}
		   // Amos