#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern TIM_HandleTypeDef htim4;

void pwm_init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

