#include "sys.h"
#include "usart.h"
#include "delay.h"
	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
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


 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
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
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}



  u8 Data_Flag=0;                      //����һ��ָ��ָ���ֽ�Ԫ��
	u8 ReseiveData;										//����һ�����������������б�����
	u8 Donwload_Data[100];	            //��ʱ��ջ�ռ����ڴ洢�ɿ���������     һ��99���ֽ�
void USART1_IRQHandler(void)                	//����1�жϷ������
	{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif

#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit(); 
       
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�  /***************************************��������**************************************/
		{	
					ReseiveData=USART_ReceiveData(USART1);
			    USART_SendData(USART3,ReseiveData);
					while((USART3->SR&0X40)==0);      ////�ȴ����ͽ���/////////////////
		}																									/***********��ɿ������ж������ݣ��·�Э�����֡��ʼ������ͬ�����Բ���͸����ʽ�·�������֤����***************/	

#endif
} 
#endif	
//////////////////////////////////////////////////////////////////////////////////////////
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
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
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_2;//2��ֹͣλ
	USART_InitStructure.USART_Parity =  USART_Parity_Even;//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
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
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//�������ڿ����ж�       //
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�       //
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3              //
	
}

	  u8	USART3_Data[100];       //���ڽ��ջ���
	  u8  DataSend_MixToUsart1[100];   //���ڷ��ͻ���
	  u8  USART2_Counter;
		u8  Data_SendParr=0;     //����һ������ָ�������Զ�����Ҫ���������֡
	  uint8_t USART3_RX_STA;
		
void USART3_IRQHandler(void)                	//����3�жϷ������		
	{
		uint8_t Clear=Clear;//���ַ�������������������"û���õ�"����
		uint8_t i=0;
		uint8_t j=i+3;
		uint8_t kk;
	
	   if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)               /************************��������****������3�н��յ�������ͨ������1ת����ȥ��**************************/
		 {
				USART3_Data[USART2_Counter++]=USART3->DR;  //�����յ��ֽڱ��棬�����ַ��1
			  //USART_SendData(USART3,0x10);
			}
		 else if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)   //������յ�һ֡����
		 {
					Clear=USART3->SR;//��SR�Ĵ���
					Clear=USART3->DR; //��ȡDR�Ĵ������ȶ�SR�ٶ�DR��Ϊ�����IDLE�жϣ�
				  USART3_RX_STA=1;
		 }
		 /*******************�����¸Ĵ�����ղ���������************************/
		 /*************���ڻ�δ�Ӷ���֡�ֽ�ͷβ������**************/
	
		 if(USART3_RX_STA==1)
		 {
				USART3_RX_STA=0;
				Data_SendParr=USART2_Counter+3;
			  /**********��ʱ����һ���µ��������������İ����ð������Ҷ���İ�ͷ�Լ���֡���ݳ��ȣ�USART2_Counter����*/
			 /*ǰ�����ֽ�Ϊ�ҵİ�ͷ���������ֽ�Ϊ������֡���ȣ�USART2_Counter��**********/
			  DataSend_MixToUsart1[0]=0x2f;			//ascii   "/"
				DataSend_MixToUsart1[1]=0x2f;     //ascii   "/"
				DataSend_MixToUsart1[2]=USART2_Counter;
//				USART_SendData(USART3,0x40);
//				while(USART_GetFlagStatus(USART3,USART_FLAG_TC) ==  RESET)			
				i=0;
				while(USART2_Counter--)
				{
						DataSend_MixToUsart1[j]=USART3_Data[i];
						i++;
						j++;
				}
				kk=0;
				while(j--)      
				{
						USART_SendData(USART2,DataSend_MixToUsart1[kk++]);      //�������ݷ���
						while(USART_GetFlagStatus(USART2,USART_FLAG_TC) ==  RESET);////�ȴ����ͽ���/////////////////
					  
						//while((USART1->SR&0X40)==0);      ////�ȴ����ͽ���/////////////////
				}
				 USART2_Counter=0;
				 delay_ms(4);
		 }
	}
	
	
	
	
	
	/*********************************����4���ó���***************************************/
 void uart4_init(u32 bound)								//
	 {								
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��USART3��GPIOBʱ��       //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	    
	//USART4_TX   GPIOC.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOB.10
  //USART4_RX	  GPIOC.11��ʼ��                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������              //
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOB.11                         //

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

