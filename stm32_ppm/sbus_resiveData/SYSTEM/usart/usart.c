#include "sys.h"
#include "usart.h"	 
#include "misc.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

////////////////////////////////////////////// 
void uart1_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
  //USART1_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA.10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity =  USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

	u8 Freame;      //中间变量
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
                      
    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
		{	

			          Freame = USART_ReceiveData(USART1);			
								USART_SendData(USART1,Freame);
					      while((USART1->SR&0X40)==0);      ////等待发送结束      

		}

}

#if EN_USART2_RX   //如果使能了接收    使能接收
//串口2中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);         //使能gpio A
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);         // 使能USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟

	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2              //引脚配置
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
  //USART1_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  
	
	
  //Usart1 NVIC 配置                  优先级配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity =  USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
	 USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启串口空闲中断       //
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断

  USART_Cmd(USART2, ENABLE);                    //使能串口2 
}

//u8 fre;
//u8 data[100];
// u8 USART2_RX_STA;
//  void USART2_IRQHandler(void)                	//串口1中断服务程序
//{
//     uint8_t Clear=Clear;                 
//			uint8_t i=0;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
//		{	

//			         data[fre++] = USART_ReceiveData(USART2);			
// 		}
//		 else if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)   //如果接收到一帧数据
//					 {
//								Clear=USART2->SR;//读SR寄存器
//								Clear=USART2->DR; //读取DR寄存器（先读SR再读DR，为了清除IDLE中断）
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
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
    uint8_t Clear=Clear;                  
		uint8_t kk;
			if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
				{	
									 Res_Buff[ResFlag++] = USART_ReceiveData(USART2);			
				}
				 else if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)   //如果接收到一帧数据
					 {
								Clear=USART2->SR;//读SR寄存器
								Clear=USART2->DR; //读取DR寄存器（先读SR再读DR，为了清除IDLE中断）
								USART2_RX_STA=1;
					 }
					 //////////////////
						 if(USART2_RX_STA==1) //接收到帧状态判断
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
																USART_SendData(UART4,Res_Buff[kk++]);                       //发送数传至飞控
															  while(USART_GetFlagStatus(UART4,USART_FLAG_TC) ==  RESET);//等待发送结束///
															  
														}
														   
													} 
											}
											ResFlag=0;								//注意此时一定要注意清除缓存地址。！！！！！！！！！！！！！！！！
											
								 }
}

/*********************************串口3配置程序***************************************/
 void uart3_init(u32 bound)								//
	 {								
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOB时钟       //
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	    
	//USART3_TX   GPIOB.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.10
  //USART3_RX	  GPIOB.11初始化                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入              //
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.11                         //

  //Usart3 NVIC 配置                                   //
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;        //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3         //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3              //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器                 //
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity =  USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口3               //
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断       //
  USART_Cmd(USART3, ENABLE);                    //使能串口3              //
	
}



#endif	

 /*********************************串口4配置程序***************************************/
 void uart4_init(u32 bound)								//
	 {								
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能USART4，GPIOB时钟       //
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	    
	//USART4_TX   GPIOC.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC.10
  //USART4_RX	  GPIOC.11初始化                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入              //
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 
  GPIO_Init(GPIOC,&GPIO_InitStructure);//初始化GPIOB.11                         //

  //Usart4 NVIC 配置                                   //
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;        //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3         //
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3              //
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器                 //
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity =  USART_Parity_No;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure); //初始化串口4               //
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断       //
  USART_Cmd(UART4, ENABLE);                    //使能串口4              //
	
}
 u8 Usart4_TO_main;
void UART4_IRQHandler(void)                	//串口4中断服务程序
	{
			  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
				{
						
			          Usart4_TO_main = USART_ReceiveData(UART4);			
								USART_SendData(USART2,Usart4_TO_main);
					      while((USART2->SR&0X40)==0);      ////等待发送结束
				}
	}
