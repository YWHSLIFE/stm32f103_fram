#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA

#define LCD_RES_Pin GPIO_PIN_0
#define LCD_RES_GPIO_Port GPIOB

#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOB

void LCD_CS_ON(void);
void LCD_CS_OFF(void);
void LCD_RES_ON(void);
void LCD_RES_OFF(void);
void LCD_DC_ON(void);
void LCD_DC_OFF(void);
#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

