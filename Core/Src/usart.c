#include "usart.h"
#include <rthw.h>
#include <rtthread.h>

void ring_buffer_init(ring_buffer *dst_buf)
{
    dst_buf->pW = 0;
    dst_buf->pR = 0;
}

void ring_buffer_write(unsigned char c, ring_buffer *dst_buf)
{
    int i = (dst_buf->pW + 1) % BUFFER_SIZE;
    if(i != dst_buf->pR)    // 环形缓冲区没有写满
    {
        dst_buf->buffer[dst_buf->pW] = c;
        dst_buf->pW = i;
    }
}

int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf)
{
    if(dst_buf->pR == dst_buf->pW)
    {
        return -1;
    }
    else
    {
        *c = dst_buf->buffer[dst_buf->pR];
        dst_buf->pR = (dst_buf->pR + 1) % BUFFER_SIZE;
        return 0;
    }
}

UART_HandleTypeDef huart1;

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(uartHandle->Instance==USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
