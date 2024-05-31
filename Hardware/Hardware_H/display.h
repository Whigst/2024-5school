#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "sys.h"
#include "lcd.h"

void LCD_ShowNumf(u16 x, u16 y, int32_t num, u8 len, u8 size);
void display(void);
void display_str(void);
void display_data(void);



#endif


