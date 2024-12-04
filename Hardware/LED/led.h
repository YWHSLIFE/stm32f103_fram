#ifndef _LED_H_
#define _LED_H_

#define LED_PORT            GPIOC
#define LED_PIN             GPIO_PIN_13

#define LED_ON()      HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_RESET);
#define LED_OFF()     HAL_GPIO_WritePin(LED_PORT,LED_PIN,GPIO_PIN_SET);
#define LED_TOGGLE()  HAL_GPIO_TogglePin(LED_PORT,LED_PIN);

int LED_init(void);
void led_proc(void);

#endif
