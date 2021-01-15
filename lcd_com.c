#include "REG_89C51.h"							 
#include "type_var_H.h"
#include "math.h"

#define RS_lcd     P2_0
#define RW_lcd     P2_1
#define E_lcd      P2_2
#define lcd_port   P2   //2.3 2.4 2.5 2.6

static u8 data lcd_num ;  // lcd position on line (0-15)

static bit lcd_line ;       // lcd line: 0 - line 1

void delay_lcd(u16 n)  	//short delay for execute commands
{						//or other delay
	u16 data i,m;

	for(m=0;m<n;m++)
		for(i=0;i<5;i++);
}//delay_lcd

//the first time the lcd powered on it's on 8 bits mode so the first command should include the 
//bit for 4 bits mode. after that the command for full initialized should send again.

void set_mode(bit RS_bit,u8 comm)
{   
	RW_lcd = 0 ; 		
   	E_lcd = 1 ;
   	RS_lcd = RS_bit ;
   	lcd_port = lcd_port & 0x87; //download data bits from P2 to 0. 0x87 = 1000 0111
	lcd_port = lcd_port | ((comm & 0xf0 )>>1);  //write to lcd the command for 4 bits mode
	E_lcd = 0 ;
   	delay_lcd(1);
	//now writing again the full mode for LCD including lines and digits
   	E_lcd = 1 ;
   	delay_lcd(1);
}


void set_lcd(bit RS_bit,u8 comm) 
{ 
   RW_lcd = 0 ; 		
   E_lcd = 1 ;
   RS_lcd = RS_bit ;
   lcd_port =lcd_port & 0x87; //clearing the data bits
   lcd_port = lcd_port | ((comm & 0xf0 )>>1);
   E_lcd = 0 ;
   delay_lcd(1);
   E_lcd = 1 ;
  
   RW_lcd = 0 ; 		
   E_lcd = 1 ;
   RS_lcd = RS_bit ;
   lcd_port = lcd_port & 0x87;
   lcd_port = lcd_port | ((comm &0x0f )<<3) ;
   E_lcd = 0 ;
   delay_lcd(1);
   E_lcd = 1 ;
   delay_lcd(1);
}


void setup_lcd() 
{

   	lcd_num  = 0 ;
   	lcd_line = 0 ;

	delay_lcd(10);
	set_mode(0,0x30);// set 8 bits mode 30 = 00110000
	set_mode(0,0x30);// set 8 bits mode 2c = 00110000
	set_mode(0,0x30);// set 8 bits mode 2c = 00110000

	set_mode(0,0x2C);// set 4 bits mode 2c = 00101100
	set_lcd(0,0x2c); // set 4 bits mode, 2 lines and 7X10 pix.

   	set_lcd(0,0x0c);//  0 0 0 0 1 D C B     - DISPLAY ON/OFF CONTROL
	set_lcd(0,0x06);//  0 0 0 0 0 1 I/D SH  - ENTRY MODE SET
	set_lcd(0,0x14);//  0 0 0 1 S/C R/L 0 0 - CURSOR OR DISPLAY SHIFT
	set_lcd(0,0x01);//  0 0 0 0 0 0 0 1     - DISPLAY CLEAR
}

void clear_lcd()
{
   lcd_num = 0 ;
   lcd_line = 0 ;
   set_lcd(0,0x01);//  0 0 0 0 0 0 0 1      - DISPLAY CLEAR
}

void lcd_line1()
{
   lcd_num = 0 ;
   lcd_line = 0 ;
   set_lcd(0,0x80);//  0 0 0 0 0 0 1 0      - DISPLAY HOME
}

void lcd_line2()
{
   lcd_num = 0 ;
   lcd_line = 1 ;
   set_lcd(0,0xc0);//  1 1 0 0 0 0 0 0      - LINE 2
}

void move_cursor(u8 place)
{
    set_lcd(0,place);
}

void lcd_write(char lcd_data)
{ 
   set_lcd(1,lcd_data);
   delay_lcd(10);
   if (lcd_num == 0x0f)
   {
      if (lcd_line)
         lcd_line1();
      else  
         lcd_line2();
   }
   else 
      lcd_num++;
}

void write_text(u8 pos,u8 text_msg[32],u8 n)
{
	u8 i;

	move_cursor(pos);
	delay_lcd(150);
	for(i=0;i<n;i++)
		lcd_write(text_msg[i]);
}

void runing_text(u8 st_pos,u8 en_pos,u8 text_msg[32],u8 n)
{
	u8 i;


	do
	{
		clear_lcd();
		delay_lcd(150);

		move_cursor(st_pos);
		delay_lcd(150);

		for(i=0;i<n;i++)
			lcd_write(text_msg[i]);
		st_pos++;
		delay_lcd(1000);
	}
	while(st_pos<en_pos);	
}

void write_number(u8 pos,double num, int dig)
{
	long in,temp,l1=0,op=1;

	move_cursor(pos);
	delay_lcd(150);

	if (num<0)
	{
		lcd_write('-');
	 	num=num*-1;
	}

	in=(int)num;
	//find the integer part length
	temp=in;
	do
	{													
		l1++;
		temp/=10;
		op=op*10;					
	}
	while(temp>0);				

	do
	{
		op=op/10;
		lcd_write(in/op+'0');
		in=in%op;
	}
	while(op>1);
 
 	if(dig>0)
		lcd_write('.');

	num=num-(int)num;

	do
	{
		num=num*10;
		lcd_write(num/1+'0');
		num=(double)(num-(int)num/1);
		dig--;
	}
	while(dig>0);

}
