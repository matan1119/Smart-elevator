
//lcd_com.h//


void delay_lcd(u16 n);
void set_mode(bit RS_bit,u8 comm);
void set_lcd(bit RS_bit,u8 comm);
void setup_lcd();
void clear_lcd();
void lcd_line1();
void lcd_line2();
void move_cursor(u8 place);
void lcd_write(char lcd_data);
void write_text(u8 pos,u8 text_msg[32],u8 n);
void runing_text(u8 st_pos,u8 en_pos,u8 text_msg[32],u8 n);
void write_number(u8 pos,float num, int dig);
