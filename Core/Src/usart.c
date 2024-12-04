#include "main.h"
#include "usart.h"

unsigned char rx_data[128] = {0};
unsigned char finsh_data[128] = {0};
static ring_buffer uart1_rx_buf = {0, 0, {0}};		/* 串口接收 ringbuffer */
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */

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

#ifdef RT_USING_CONSOLE
static int rt_console_init(void)
{
    ring_buffer_init(&uart1_rx_buf);
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_NVIC_SetPriority(USART1_IRQn ,19,0);
    HAL_NVIC_EnableIRQ(USART1_IRQn ); /*使能USART1中断通道*/

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_data, sizeof(rx_data));
    return 0;
}
INIT_BOARD_EXPORT(rt_console_init);

void rt_hw_console_output(const char *str)
{
    // rt_size_t i = 0;
    rt_size_t size = 0;
    // char a = '\r';

    __HAL_UNLOCK(&huart1);

    size = rt_strlen(str);

    // for (i = 0; i < size; i++)
    // {
    //     if (*(str + i) == '\n')
    //     {
    //         HAL_UART_Transmit(&huart1, (uint8_t *)a, 1, 1);
    //     }
    //     HAL_UART_Transmit(&huart1, (uint8_t *)(str + i), 1, 1);
    // }
    HAL_UART_Transmit(&huart1, (uint8_t *)(str), size, 10);
}
#endif

#ifdef RT_USING_FINSH
char rt_hw_console_getchar(void)
{
    char ch = 0;
    while (ring_buffer_read((unsigned char *)&ch, &uart1_rx_buf) != 0)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}
#endif

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    unsigned char ch;
	unsigned char i = 0;
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if (huart->Instance == USART1) 
    {
        while(Size--)
        {
            ch = rx_data[i++];
            /* 读取到数据，将数据存入 ringbuffer */
            ring_buffer_write(ch, &uart1_rx_buf);
        }
        rt_memset(rx_data,0,sizeof(rx_data));
        rt_sem_release(&shell_rx_sem);
    }
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_data, sizeof(rx_data));
}
