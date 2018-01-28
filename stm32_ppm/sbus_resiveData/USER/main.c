#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dma.h"
// STM32           C8T6
//2017.9.18
//串口  

  
u8  Counter=0;          //SBus 计数
u8 State=0;
int main(void)
{	 
			delay_init();	    	 //延时函数初始化	
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
					uart1_init(100000);	 //串口1初始化为100000		
					uart2_init(115200);	 //串口2初始化为115200
					uart3_init(115200);	 //串口3初始化为115200	
		      uart4_init(115200);  //串口4初始化为115200
			while(1)
			{ 
				  for(Counter=0;Counter<25;Counter++)
					{
					    USART_SendData(USART1,Sbus_TO_SEND[Counter]);
							while((USART1->SR&0X40)==0);      ////等待发送结束
					}
					delay_ms(11);         
			}
}
