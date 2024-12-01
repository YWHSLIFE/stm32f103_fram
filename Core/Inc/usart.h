/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 256        /* 环形缓冲区的大小 */

typedef struct
{
    volatile unsigned int pR;           /* 读地址 */
    volatile unsigned int pW;           /* 写地址 */   
    unsigned char buffer[BUFFER_SIZE];  /* 缓冲区空间 */    
} ring_buffer;

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern UART_HandleTypeDef huart1;
void ring_buffer_init(ring_buffer *dst_buf);
int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf);
void ring_buffer_write(unsigned char c, ring_buffer *dst_buf);
#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

