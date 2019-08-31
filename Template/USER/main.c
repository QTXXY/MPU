#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "iic.h"
#include "mpu6050.h"

u8 MpuState = 1; // 

unsigned char Who(unsigned char *Addr)
{
	return MasterReadDevice(0xD0,0x75,1,Addr);
}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	InitDelay();
	InitUart(115200);
	InitIic();	
	MpuState = MPU_Init();
	while(1)
	{	
		printf("%d\r\n",MpuState);
		DelayMs(500);		
	}	 
} 

