#include "main.h"
#include "led.h"

void led_proc(void)
{
	LED_TOGGLE();
}

static int LED_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
 	
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = LED_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

	LED_OFF();
	return 0;
}
INIT_BOARD_EXPORT(LED_init);

static void led_test(int num,char **p)
{
	int cmd;
	cmd = atoi(p[1]);
	if(cmd)
	{
		LED_ON();
	}
	else
	{
		LED_OFF();
	}
}
MSH_CMD_EXPORT(led_test,led_test1);
