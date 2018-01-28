#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************/
extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ	                  TIM5CH1_CAPTURE_VAL
extern u16 ppm_rx[];

extern u8  TIM2CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ	
extern u16 ppm2_rx[];

		u16 channels[16];
		u8 Sbus[25]={0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
		u8 channels_flag=0;
		u8 Sbus_Flag=0;
/****************����3�������ò���ʼ��****************************************/
//		volatile u8 USART3_Data[100]={0x00};
//		volatile u8  USART3_Counter=0;
//		volatile uint8_t USART3_RX_STA=0;
/******************************************************/			
 int main(void)
 {			 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(100000);	 //���ڳ�ʼ��Ϊ100000    83520
	uart2_init(115200);  //����2��ʼ��Ϊ115200
	uart3_init(115200); 
	uart4_init(115200); 
 	LED_Init();			     //LED�˿ڳ�ʼ��
 	TIM3_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ��� 
	TIM4_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ���  
	
	 
   	while(1)
	{
	
 		if(ppm_rx[0])//�ɹ�������һ��������              ////��ң��PPM�ź� ��ʱ��3
		{
			for(channels_flag=0;channels_flag<8;channels_flag++)
			{
				channels[channels_flag]=ppm_rx[channels_flag+1];	
			}
			channels_flag=0;
			//printf("����:%d ǰ��:%d ����:%d ����:%d chanel5:%d chanel6:%d chanel7:%d chanel8:%d\r\n",channels[0],channels[1],channels[2],channels[3],channels[4],channels[5],channels[6],channels[7]);
	
			ppm_rx[0]=0;
		}
			
		 
		if(ppm2_rx[0])     //��ң��PPM�ź�/��ʱ��4///////////////////////////////////////////////////////////��
		{
//printf("R&L:%d G&B:%d ����:%d ����:%d chanel5:%d chanel6:%d chanel7:%d chanel8:%d\r\n",ppm2_rx[1],ppm2_rx[2],ppm2_rx[3],ppm2_rx[4],ppm2_rx[5],ppm2_rx[6],ppm2_rx[7],ppm2_rx[8]);
			for(channels_flag=0;channels_flag<9;channels_flag++)
			{
			   channels[channels_flag+8]=ppm2_rx[channels_flag+1];
			}				
			channels_flag=0;
			ppm2_rx[0]=0;
		}
		
		
		  Sbus[0]=0x0f;    //Sbus��ͷ��/
//////////////////////////////////ǰ��ͨ��/////////////////////////////////////////			
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
///////////////////////////////////���ͨ��/////////////////////////////////////////			
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
			Sbus[23]=0x00;	 //�����ڶ�λУ��λ���ź�λ
			Sbus[24]=0x00;   //SBUS��β��
		

		if(Sbus[1]==0&&Sbus[2]==0&&Sbus[3]==0&&Sbus[4]==0&&Sbus[5]==0&&Sbus[6]==0&&Sbus[7]==0&&Sbus[8]==0){Sbus[23]=0x20;}     //֡��ʧ������
			/*******֡��ʧ��������Ҫ��ȡP400���źŵ�״̬��������һ���յ�*********/	  
    for(Sbus_Flag=0;Sbus_Flag<25;Sbus_Flag++)     /*��ʼ����sbus�ź�*/
		{
				USART_SendData(USART2,Sbus[Sbus_Flag]);
				while((USART2->SR&0X40)==0);      ////�ȴ����ͽ���/////////////////
				
		}
		delay_ms(11);

	}
 }

