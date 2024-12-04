#include "main.h"
#include "spi.h"
#include "oled.h"

struct rt_thread oled_thread;
static rt_uint8_t oled_stack[RT_MAIN_THREAD_STACK_SIZE/2];

void OLED_Init(void)
{
	OLED_RST_Set(); 
	rt_thread_delay(200);
	OLED_RST_Clr();
	rt_thread_delay(200);
	OLED_RST_Set(); 

	OLED_CS_Clr();
	OLED_DC_Clr();
	
	oled_write_byte(0xAE); //关闭显示
	oled_write_byte(0xD5); //设置时钟分频因子,震荡频率
	oled_write_byte(0x80); //[3:0],分频因子;[7:4],震荡频率
	oled_write_byte(0xA8); //设置驱动路数
	oled_write_byte(0X3F); //默认0X3F(1/64) 
	oled_write_byte(0xD3); //设置显示偏移
	oled_write_byte(0X00); //默认为0

	oled_write_byte(0x40); //设置显示开始行 [5:0],行数.
													
	oled_write_byte(0x8D); //电荷泵设置
	oled_write_byte(0x14); //bit2，开启/关闭
	oled_write_byte(0x20); //设置内存地址模式
	oled_write_byte(0x00); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_write_byte(0xA1); //段重定义设置,bit0:0,0->0;1,0->127;
	oled_write_byte(0xC8); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_write_byte(0xDA); //设置COM硬件引脚配置
	oled_write_byte(0x12); //[5:4]配置
		 
	oled_write_byte(0x81); //对比度设置
	oled_write_byte(0xEF); //1~255;默认0X7F (亮度设置,越大越亮)
	oled_write_byte(0xD9); //设置预充电周期
	oled_write_byte(0xf1); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_write_byte(0xDB); //设置VCOMH 电压倍率
	oled_write_byte(0x30); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_write_byte(0xA4); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_write_byte(0xA6); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	oled_write_byte(0xAF); //开启显示	 	

	OLED_DC_Set();

	OLED_ClearBuff();
	OLED_SetPointColor(1); //设置绘制颜色
}

void oled_thread_entry(void *parameter)
{
	OLED_Init();
    OLED_WinDrawStr(&oled_win,0,0,16,"hungry world!");
	while(1)
	{
        OLED_RefreshBuff();
		rt_thread_delay(20);
	}
}

void rt_oled_init(void)
{
    rt_thread_t tid;

#ifdef RT_USING_HEAP
    tid = rt_thread_create("oled", oled_thread_entry, RT_NULL,
                           RT_MAIN_THREAD_STACK_SIZE, 30, 20);
    RT_ASSERT(tid != RT_NULL);
#else
    rt_err_t result;

    tid = &oled_thread;
    result = rt_thread_init(tid, "oled", oled_thread_entry, RT_NULL,
                            oled_stack, sizeof(oled_stack), 30, 20);
    RT_ASSERT(result == RT_EOK);
#endif
    rt_thread_startup(tid);
}
