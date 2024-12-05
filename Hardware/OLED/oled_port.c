#include "oled_port.h"
#include "oled.h"
#include "spi.h"
#include "string.h"
#include "main.h"

#define OLED_CMD 	0
#define OLED_DATA 	1

void oled_write_byte(uint8_t Byte)
{
	HAL_SPI_Transmit(&hspi1, &Byte, 1, 50);
}
static void OLED_WriteArray(uint8_t* dat)
{
	// HAL_SPI_Transmit(&hspi1,(uint8_t *)dat,OLED_BUFF_SIZE, 1000);
	HAL_SPI_Transmit_IT(&hspi1,(uint8_t *)dat,OLED_BUFF_SIZE);
}
void OLED_SendBuff(uint8_t *buff)
{		    
	OLED_WriteArray(buff);
}
