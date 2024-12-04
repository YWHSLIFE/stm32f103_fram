#ifndef __OLED_H
#define __OLED_H 

#include "gpio.h"
#include "oled_g.h"
#include "oled_port.h"

#define OLED_MODE   ( 1 )

#if OLED_MODE == 0//iic

#else

#define OLED_RST_Clr() 	LCD_RES_OFF()
#define OLED_RST_Set() 	LCD_RES_ON()

#define OLED_DC_Clr() 	LCD_DC_OFF()
#define OLED_DC_Set() 	LCD_DC_ON()
 		     
#define OLED_CS_Clr()  	LCD_CS_OFF()
#define OLED_CS_Set()  	LCD_CS_ON()

#endif

void OLED_Init(void);
void rt_oled_init(void);

#endif

