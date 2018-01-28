#include "sys.h"
#include "usart.h"
#include "delay.h"
	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
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


 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void uart_init(u32 bound){
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
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}



  u8 Data_Flag=0;                      //定义一个指针指向字节元素
	u8 ReseiveData;										//定义一个常量接收数传下行变量。
	u8 Donwload_Data[100];	            //临时存栈空间用于存储飞控下行数据     一共99个字节
void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif

#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit(); 
       
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断  /***************************************数传下行**************************************/
		{	
					ReseiveData=USART_ReceiveData(USART1);
			    USART_SendData(USART3,ReseiveData);
					while((USART3->SR&0X40)==0);      ////等待发送结束/////////////////
		}																									/***********因飞控下行有多重数据，下发协议各类帧起始不尽相同。所以采用透传形式下发不作验证处理。***************/	

#endif
} 
#endif	
//////////////////////////////////////////////////////////////////////////////////////////
void uart2_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
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
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_2;//2个停止位
	USART_InitStructure.USART_Parity =  USART_Parity_Even;//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断

  USART_Cmd(USART2, ENABLE);                    //使能串口2 
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
  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启串口空闲中断       //
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断       //
  USART_Cmd(USART3, ENABLE);                    //使能串口3              //
	
}

	  u8	USART3_Data[100];       //串口接收缓存
	  u8  DataSend_MixToUsart1[100];   //串口发送缓存
	  u8  USART2_Counter;
		u8  Data_SendParr=0;     //创建一个过渡指针修饰自定义需要封包的数据帧
	  uint8_t USART3_RX_STA;
		
void USART3_IRQHandler(void)                	//串口3中断服务程序		
	{
		uint8_t Clear=Clear;//这种方法用来消除编译器的"没有用到"提醒
		uint8_t i=0;
		uint8_t j=i+3;
		uint8_t kk;
	
	   if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)               /************************数传上行****将串口3中接收到的数据通过串口1转发出去。**************************/
		 {
				USART3_Data[USART2_Counter++]=USART3->DR;  //将接收的字节保存，数组地址加1
			  //USART_SendData(USART3,0x10);
			}
		 else if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)   //如果接收到一帧数据
		 {
					Clear=USART3->SR;//读SR寄存器
					Clear=USART3->DR; //读取DR寄存器（先读SR再读DR，为了清除IDLE中断）
				  USART3_RX_STA=1;
		 }
		 /*******************测试新改代码接收不定长数据************************/
		 /*************现在还未加定义帧字节头尾方法。**************/
	
		 if(USART3_RX_STA==1)
		 {
				USART3_RX_STA=0;
				Data_SendParr=USART2_Counter+3;
			  /**********此时创建一个新的数组容纳我做的包，该包包含我定义的包头以及该帧数据长度（USART2_Counter）。*/
			 /*前两个字节为我的包头，第三个字节为该数据帧长度（USART2_Counter）**********/
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
						USART_SendData(USART2,DataSend_MixToUsart1[kk++]);      //上行数据发送
						while(USART_GetFlagStatus(USART2,USART_FLAG_TC) ==  RESET);////等待发送结束/////////////////
					  
						//while((USART1->SR&0X40)==0);      ////等待发送结束/////////////////
				}
				 USART2_Counter=0;
				 delay_ms(4);
		 }
	}
	
	
	
	
	
	/*********************************串口4配置程序***************************************/
 void uart4_init(u32 bound)								//
	 {								
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能USART3，GPIOB时钟       //
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);	    
	//USART4_TX   GPIOC.10                																	//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10  									//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOB.10
  //USART4_RX	  GPIOC.11初始化                    //
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11            //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入              //
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	 
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOB.11                         //

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

