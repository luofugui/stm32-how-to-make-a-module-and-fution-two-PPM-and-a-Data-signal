#include "sys.h"
#include "usart.h"	 
#include "misc.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

////////////////////////////////////////////// 
void uart1_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
  //USART1_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA.10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity =  USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}

	u8 Freame;      //�м����
void USART1_IRQHandler(void)                	//����1�жϷ������
{
                      
    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
		{	

			          Freame = USART_ReceiveData(USART1);			
								USART_SendData(USART1,Freame);
					      while((USART1->SR&0X40)==0);      ////�ȴ����ͽ���      

		}

}

#if EN_USART2_RX   //���ʹ���˽���    ʹ�ܽ���
//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);         //ʹ��gpio A
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);         // ʹ��USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��

	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2              //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
  //USART1_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  
	
	
  //Usart1 NVIC ����                  ���ȼ�����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity =  USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	 USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//�������ڿ����ж�       //
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}

//u8 fre;
//u8 data[100];
// u8 USART2_RX_STA;
//  void USART2_IRQHandler(void)                	//����1�жϷ������
//{
//     uint8_t Clear=Clear;                 
//			uint8_t i=0;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
//		{	

//			         data[fre++] = USART_ReceiveData(USART2);			
// 		}
//		 else if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)   //������յ�һ֡����
//					 {
//								Clear=USART2->SR;//��SR�Ĵ���
//								Clear=USART2->DR; //��ȡDR�Ĵ������ȶ�SR�ٶ�DR��Ϊ�����IDLE�жϣ�
//								USART2_RX_STA=1;
//					 }
//					 
//			if( USART2_RX_STA==1)
//			{
//				     USART2_RX_STA=0;
//			
//							while(fre--)
//							{
//									USART_SendData(UART4,data[i++]);
//									while(USART_GetFlagStatus(UART4,USART_FLAG_TC) ==  RESET);
//							}
//							fre=0;
//			}				

//}

uint8_t USART2_RX_STA;
u8 Res_Buff[100];
uint8_t ResFlag;
uint8_t Data_flag;
uint8_t SBUS_Flag=0;
u8  Sbus_TO_SEND[25];
void USART2_IRQHandler(void)                	//����1�жϷ������
{
    uint8_t Clear=Clear;                  
		uint8_t kk;
			if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
				{	
									 Res_Buff[ResFlag++] = USART_ReceiveData(USART2);			
				}
				 else if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)   //������յ�һ֡����
					 {
								Clear=USART2->SR;//��SR�Ĵ���
								Clear=USART2->DR; //��ȡDR�Ĵ������ȶ�SR�ٶ�DR��Ϊ�����IDLE�жϣ�
								USART2_RX_STA=1;
					 }
					 //////////////////
						 if(USART2_RX_STA==1) //���յ�֡״̬�ж�
								 {   
									 USART2_RX_STA=0;
										if(ResFlag==25)
											{
													if(Res_Buff[0]==0x0f&&Res_Buff[24]==0x00)
													{
														 while(ResFlag--)
														 {
																Sbus_TO_SEND[SBUS_Flag]  = Res_Buff[SBUS_Flag];
															  SBUS_Flag++;
														 }
													}
											}
											else {
												Data_flag=ResFlag-3;
											  if(Res_Buff[0]==0x2f&&Res_Buff[1]==0x2f&&Res_Buff[2]==Data_flag)
													{
														kk=3;
														while(Data_flag--)
														{
																USART_SendData(UART4,Res_Buff[kk++]);                       //�����������ɿ�
															  while(USART_GetFlagStatus(UART4,USART_FLAG_TC) ==  RESET);//�ȴ����ͽ���///
															  
														}
														   
													} 
											}
											ResFlag=0;								//ע���ʱһ��Ҫע����������ַ����������������������������������
											
								 }
}

/*********************************����3���ó���***************************************/
 void uart3_init(u32 bound)								//
	 {								
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOBʱ��       //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	    
	//USART3_TX   GPIOB.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.10
  //USART3_RX	  GPIOB.11��ʼ��                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������              //
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.11                         //

  //Usart3 NVIC ����                                   //
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;        //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3         //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3              //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���                 //
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity =  USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������3               //
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�       //
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3              //
	
}



#endif	

 /*********************************����4���ó���***************************************/
 void uart4_init(u32 bound)								//
	 {								
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��USART4��GPIOBʱ��       //
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	    
	//USART4_TX   GPIOC.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10
  //USART4_RX	  GPIOC.11��ʼ��                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������              //
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 
  GPIO_Init(GPIOC,&GPIO_InitStructure);//��ʼ��GPIOB.11                         //

  //Usart4 NVIC ����                                   //
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;        //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3         //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3              //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���                 //
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity =  USART_Parity_No;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������4               //
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�       //
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4              //
	
}
 u8 Usart4_TO_main;
void UART4_IRQHandler(void)                	//����4�жϷ������
	{
			  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
				{
						
			          Usart4_TO_main = USART_ReceiveData(UART4);			
								USART_SendData(USART2,Usart4_TO_main);
					      while((USART2->SR&0X40)==0);      ////�ȴ����ͽ���
				}
	}
