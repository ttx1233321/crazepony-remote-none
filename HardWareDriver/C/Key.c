#include "Key.h"
#include "Led.h"
/*    
      ____                      _____                  +---+
     / ___\                     / __ \                 | R |
    / /                        / /_/ /                 +---+
   / /   ________  ____  ___  / ____/___  ____  __   __
  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
 / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
 \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
                                                 / /
                                            ____/ /
                                           /_____/
Tim.c file
编写者：小马  (Camel)
作者E-mail：375836945@qq.com
编译环境：MDK-Lite  Version: 4.23
初版时间: 2014-01-28
功能：
1.按键IO口初始化
2.这部分按键的功能是对外开放的，我没有定义 其功能，大家自由发挥吧
3.遥控上的Key1暂时不用，余下的三个按键自定义，可以设置为偏航调节，一键动作等等
------------------------------------
*/


extern int  Pitch_Offest;
extern int  Roll_Offest;

/********************************************
              Key初始化函数
功能：
1.配置Key接口IO输入方向

描述：
Key接口：
Key1-->PB2
Key2-->PB1
Key3-->PB3
********************************************/
void KeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PB1,PB3,PA8) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);

	/* config the NVIC(Key2-->PB1) */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* config the NVIC(Key3-->PB3) */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* config the NVIC(Key4-->PA8) */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure Key2-->PB1 as output push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure Key3-->PB3 as output push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure Key4-->PA8 as output push-pull */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	//Key2-->PB1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;            //设定外部中断1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //设定中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //设定下降沿触发模式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//Key3-->PB3
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;            //设定外部中断1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //设定中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //设定下降沿触发模式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	//Key4-->PA8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;            //设定外部中断1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //设定中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //设定下降沿触发模式
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

int key_mode = 0;	//标示现在按键所处模式，由key2进行切换。0：微调pitch模式，1：微调roll模式
int offset_flag = 0;

void EXTI1_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line1) != RESET) //确保是否产生了EXTI Line中断
	{
		if(0 == key_mode){
			key_mode = 1;
			Led2 = 1;	//Led2亮起标示进入了微调roll的模式
		}else{
			key_mode = 0;
			Led2 = 0;	//Led2灭掉标示进入了微调pitch的模式
		}
		printf("key 2\n");
		EXTI_ClearITPendingBit(EXTI_Line1);     //清除中断标志位
	}
}

void EXTI3_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line3) != RESET) //确保是否产生了EXTI Line中断
	{
		if(0 == key_mode)	//微调pitch
		{
			Pitch_Offest++;
		}else{	//微调roll
			Roll_Offest++;
		}

	 	Led3 = 1;
		offset_flag = 1;
		printf("key 3\n");
		EXTI_ClearITPendingBit(EXTI_Line3);     //清除中断标志位
	}
}

void EXTI9_5_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line8) != RESET) //确保是否产生了EXTI Line中断
	{	    
		if(0 == key_mode)	//微调pitch
		{
			Pitch_Offest--;
		}else{	//微调roll
			Roll_Offest--;
		}

	 	Led4 = 1;
		offset_flag = 1;
		printf("key 4\n");
		EXTI_ClearITPendingBit(EXTI_Line8);     //清除中断标志位
	}
}




