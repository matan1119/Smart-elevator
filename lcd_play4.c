//===========================================================================
//                        "lcd_play.c"
//===========================================================================

//===========================================================================
//   LCD in 4 bit mode only		
// Intercommunion between LCD and Microcontroller
//
// E    (Enable)              --------  P3_5
// R/W  (Read / Write)        --------  P3_6
// RS   (Character / Command) --------  P3_7    
// D4-7 (4bits data)          --------  P2    
//===========================================================================

//===========================================================================

//#include "REG_T89C51RD2_68.h"
//#include "c_type.h"
#include "REG_89C51.h"							 
#include "type_var_H.h"
#include "KeyTest.h"
#include "string.h"

#define RS_lcd     P2_0
#define RW_lcd     P2_1
#define E_lcd      P2_2
#define DATA_lcd   P2

static u8 data lcd_num ;  // lcd position on line (0-15)

static bit lcd_line ;       // lcd line: 0 - line 1
                            //           1 - line 2
void delay_lcd()
{
	u16 data i;
	for(i=0;i<750;i++);
}//delay1
//===========================================================================
void lcd_wait()
{
   u8 lcd_t1;

   for (lcd_t1 = 1 ; lcd_t1 < 5 ; lcd_t1++);   // let the display controller  
                                               // time to execute the command
}//lcd_wait()

//===========================================================================
void set_lcd1(bit char_command,u8 lcd_d) 
{ 
   RW_lcd = 0 ; 		
   E_lcd = 1 ;
   RS_lcd = char_command ;
   DATA_lcd &= 0x87; 
   DATA_lcd |= ((lcd_d &0xf0 )>>1);
   E_lcd = 0 ;
   lcd_wait();
   E_lcd = 1 ;
   lcd_wait();
}//set_lcd(bit char_command,char lcd_d)

void set_lcd(bit char_command,u8 lcd_d) 
{ 
   RW_lcd = 0 ; 		
   E_lcd = 1 ;
   RS_lcd = char_command ;
   DATA_lcd &= 0x87;
   DATA_lcd |= ((lcd_d &0xf0 )>>1);
   E_lcd = 0 ;
   lcd_wait();
   E_lcd = 1 ;
//   lcd_wait();
   RW_lcd = 0 ; 		
   E_lcd = 1 ;
   RS_lcd = char_command ;
   DATA_lcd &= 0x87;
   DATA_lcd |= ((lcd_d &0x0f )<<3) ;
   E_lcd = 0 ;
   lcd_wait();
   E_lcd = 1 ;
   lcd_wait();
}//set_lcd(bit char_command,char lcd_d)

//===========================================================================
/*
 I/D - when high, cursor moves right DDRAM addr. inc. by 1
     - when low,  cursor moves left  DDRAM addr. dec. by 1
 SH  - Shift of entire display
 D   - Display on/off (high = on; low = off; Data stored)
 C   - Cursor  on/off (high = on; low = off; ID   stored)
 B   - Cursor blink on/off (high = blink on; low = blink off)
 S/C - 1 - display; 0 - cursor;
 R/L - 1 - right;   0 - left;
 DL  - Interface data length  ( 1 - 8 bit; 0 - 4 bit;)
 N   - Display line number (1 - two lines; 0 - one line;)
 F   - Display font type (1 - 5x11; 0 - 5x8;)
*/
//===========================================================================
void lcd_setup() 
{
   u8 lcd_i;

   lcd_num  = 0 ;
   lcd_line = 0 ;

   for (lcd_i = 0 ; lcd_i < 10 ; lcd_i++)  // Recovery time after power on
   lcd_wait();

   set_lcd1(0,0x30);//  0 0 1 DL N F 0 0    - FUNCTION SET - 8bit interface
   set_lcd1(0,0x30);//  0 0 1 DL N F 0 0    - FUNCTION SET - 8bit interface
   set_lcd1(0,0x30);//  0 0 1 DL N F 0 0    - FUNCTION SET - 8bit interface
   set_lcd1(0,0x2C);//  0 0 1 DL N F 0 0    - FUNCTION SET - 4bit interface

   set_lcd(0,0x2C);//  0 0 1 DL N F 0 0    - FUNCTION SET - 4bit interface

   set_lcd(0,0x0F);//  0 0 0 0 1 D C B     - DISPLAY ON/OFF CONTROL

   set_lcd(0,0x06);//  0 0 0 0 0 1 I/D SH  - ENTRY MODE SET

   set_lcd(0,0x14);//  0 0 0 1 S/C R/L 0 0 - CURSOR OR DISPLAY SHIFT

   set_lcd(0,0x01);//  0 0 0 0 0 0 0 1     - DISPLAY CLEAR




}//lcd_setup()

void lcd_clear()
{
   lcd_num = 0 ;
   lcd_line = 0 ;
   set_lcd(0,0x01);//  0 0 0 0 0 0 0 1      - DISPLAY CLEAR
}//lcd_clear()

//===========================================================================
void lcd_home()
{
   lcd_num = 0 ;
   lcd_line = 0 ;
   set_lcd(0,0x80/*0x02*/);//  0 0 0 0 0 0 1 0      - DISPLAY HOME
}//lcd_home()

//===========================================================================
void lcd_line2()
{
   lcd_num = 0 ;
   lcd_line = 1 ;
   set_lcd(0,0xc0);//  1 1 0 0 0 0 0 0      - LINE 2
}//lcd_line2()

void GoToCursor(u8 MenuCursorPlace)
{
    set_lcd(0,MenuCursorPlace);
}

void lcd_write(u8 num)
{
    lcd_wait();

	   set_lcd(1,num);
	   lcd_wait();

}



//===========================================================================
//                   End Of File "lcd_play8.c"        
//===========================================================================