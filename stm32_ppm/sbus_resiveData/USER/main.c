#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dma.h"
// STM32           C8T6
//2017.9.18
//����  

  
u8  Counter=0;          //SBus ����
u8 State=0;
int main(void)
{	 
			delay_init();	    	 //��ʱ������ʼ��	
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
					uart1_init(100000);	 //����1��ʼ��Ϊ100000		
					uart2_init(115200);	 //����2��ʼ��Ϊ115200
					uart3_init(115200);	 //����3��ʼ��Ϊ115200	
		      uart4_init(115200);  //����4��ʼ��Ϊ115200
			while(1)
			{ 
				  for(Counter=0;Counter<25;Counter++)
					{
					    USART_SendData(USART1,Sbus_TO_SEND[Counter]);
							while((USART1->SR&0X40)==0);      ////�ȴ����ͽ���
					}
					delay_ms(11);         
			}
}
