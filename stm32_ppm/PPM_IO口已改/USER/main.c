#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************/
extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM5CH1_CAPTURE_VAL;	//输入捕获值	                  TIM5CH1_CAPTURE_VAL
extern u16 ppm_rx[];

extern u8  TIM2CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值	
extern u16 ppm2_rx[];

		u16 channels[16];
		u8 Sbus[25]={0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
		u8 channels_flag=0;
		u8 Sbus_Flag=0;
/****************串口3参数引用并初始化****************************************/
//		volatile u8 USART3_Data[100]={0x00};
//		volatile u8  USART3_Counter=0;
//		volatile uint8_t USART3_RX_STA=0;
/******************************************************/			
 int main(void)
 {			 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(100000);	 //串口初始化为100000    83520
	uart2_init(115200);  //串口2初始化为115200
	uart3_init(115200); 
	uart4_init(115200); 
 	LED_Init();			     //LED端口初始化
 	TIM3_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数 
	TIM4_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数  
	
	 
   	while(1)
	{
	
 		if(ppm_rx[0])//成功捕获到了一次上升沿              ////主遥控PPM信号 定时器3
		{
			for(channels_flag=0;channels_flag<8;channels_flag++)
			{
				channels[channels_flag]=ppm_rx[channels_flag+1];	
			}
			channels_flag=0;
			//printf("左右:%d 前后:%d 油门:%d 航向:%d chanel5:%d chanel6:%d chanel7:%d chanel8:%d\r\n",channels[0],channels[1],channels[2],channels[3],channels[4],channels[5],channels[6],channels[7]);
	
			ppm_rx[0]=0;
		}
			
		 
		if(ppm2_rx[0])     //从遥控PPM信号/定时器4///////////////////////////////////////////////////////////。
		{
//printf("R&L:%d G&B:%d 油门:%d 航向:%d chanel5:%d chanel6:%d chanel7:%d chanel8:%d\r\n",ppm2_rx[1],ppm2_rx[2],ppm2_rx[3],ppm2_rx[4],ppm2_rx[5],ppm2_rx[6],ppm2_rx[7],ppm2_rx[8]);
			for(channels_flag=0;channels_flag<9;channels_flag++)
			{
			   channels[channels_flag+8]=ppm2_rx[channels_flag+1];
			}				
			channels_flag=0;
			ppm2_rx[0]=0;
		}
		
		
		  Sbus[0]=0x0f;    //Sbus的头部/
//////////////////////////////////前八通道/////////////////////////////////////////			
			Sbus[1]=(u8)((channels[0]&0x07ff));
			Sbus[2]=(u8)((channels[0]&0x07ff)>>8|(channels[1]&0x07ff)<<3);
		  Sbus[3]=(u8)((channels[1]&0x07ff)>>5|(channels[2]&0x07ff)<<6);
			Sbus[4]=(u8)((channels[2]&0x07ff)>>2);
			Sbus[5]=(u8)((channels[2]&0x07ff)>>10|(channels[3]&0x07ff)<<1);
			Sbus[6]=(u8)((channels[3]&0x07ff)>>7|(channels[4]&0x07ff)<<4);
			Sbus[7]=(u8)((channels[4]&0x07ff)>>4|(channels[5]&0x07ff)<<7);
			Sbus[8]=(u8)((channels[5]&0x07ff)>>1);
			Sbus[9]=(u8)((channels[5]&0x07ff)>>9|(channels[6]&0x07ff)<<2);
			Sbus[10]=(u8)((channels[6]&0x07ff)>>6|(channels[7]&0x07ff)<<5);
			Sbus[11]=(u8)((channels[7]&0x07ff)>>3);
///////////////////////////////////后八通道/////////////////////////////////////////			
			Sbus[12]=(u8)((channels[8]&0x07ff));
			Sbus[13]=(u8)((channels[8]&0x07ff)>>8|(channels[9]&0x07ff)<<3);
		  Sbus[14]=(u8)((channels[9]&0x07ff)>>5|(channels[10]&0x07ff)<<6);
			Sbus[15]=(u8)((channels[10]&0x07ff)>>2);
			Sbus[16]=(u8)((channels[10]&0x07ff)>>10|(channels[11]&0x07ff)<<1);
			Sbus[17]=(u8)((channels[11]&0x07ff)>>7|(channels[12]&0x07ff)<<4);
			Sbus[18]=(u8)((channels[12]&0x07ff)>>4|(channels[13]&0x07ff)<<7);
			Sbus[19]=(u8)((channels[13]&0x07ff)>>1);
			Sbus[20]=(u8)((channels[13]&0x07ff)>>9|(channels[14]&0x07ff)<<2);
			Sbus[21]=(u8)((channels[14]&0x07ff)>>6|(channels[15]&0x07ff)<<5);
			Sbus[22]=(u8)((channels[15]&0x07ff)>>3);   
			Sbus[23]=0x00;	 //倒数第二位校验位与信号位
			Sbus[24]=0x00;   //SBUS的尾部
		

		if(Sbus[1]==0&&Sbus[2]==0&&Sbus[3]==0&&Sbus[4]==0&&Sbus[5]==0&&Sbus[6]==0&&Sbus[7]==0&&Sbus[8]==0){Sbus[23]=0x20;}     //帧丢失保护。
			/*******帧丢失保护还需要获取P400的信号灯状态。这里留一个空挡*********/	  
    for(Sbus_Flag=0;Sbus_Flag<25;Sbus_Flag++)     /*开始传输sbus信号*/
		{
				USART_SendData(USART2,Sbus[Sbus_Flag]);
				while((USART2->SR&0X40)==0);      ////等待发送结束/////////////////
				
		}
		delay_ms(11);

	}
 }

