#include "display.h"
#include "timer.h"
#include "ms53l1m.h"
#include "wit_c_sdk.h"

void display_data(void)
{
	LCD_Showfloat(90,10,fAngle[0],3,16);
  LCD_Showfloat(90,30,fAngle[1],3,16);
  LCD_Showfloat(90,50,fAngle[2],3,16);
	LCD_ShowNumf(90,70,distance,5,16);
}
void display_str(void)
{
	LCD_ShowString(10,10,210,16,16, "AngleX:");  
  LCD_ShowString(10,30,210,16,16, "AngleY:");  
  LCD_ShowString(10,50,210,16,16, "AngleZ:"); 
	LCD_ShowString(10,70,210,16,16, "distance:");
 }




