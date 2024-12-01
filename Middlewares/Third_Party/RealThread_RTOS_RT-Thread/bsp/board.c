#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "main.h"

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

extern void SystemCoreClockUpdate(void);
extern uint32_t SystemCoreClock;

unsigned char rx_data[128] = {0};
unsigned char finsh_data[128] = {0};
static ring_buffer uart1_rx_buf = {0, 0, {0}};		/* 串口接收 ringbuffer */
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */

static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }
    
    _SYSTICK_LOAD = ticks - 1; 
    _SYSTICK_PRI = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;  
    
    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif
void SystemClock_Config(void);
/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    HAL_Init();
    SystemClock_Config();
    SystemCoreClockUpdate();
    
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

#ifdef RT_USING_CONSOLE
extern UART_HandleTypeDef huart1;
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

    HAL_NVIC_SetPriority(USART1_IRQn ,20,1);
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
    rt_size_t i = 0, size = 0;
    char a = '\r';

    __HAL_UNLOCK(&huart1);

    size = rt_strlen(str);

    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            HAL_UART_Transmit(&huart1, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&huart1, (uint8_t *)(str + i), 1, 1);
    }
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
