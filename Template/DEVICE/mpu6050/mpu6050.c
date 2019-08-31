#include "mpu6050.h"

unsigned char buff[1] = {0};
/*****************************************************************************
*
*  名称:   MPU_Init
*
*  描述:   返回值 0  成功
*
*          其他值，错误代码
*****************************************************************************/
u8 MPU_Init(void)
{
	u8 res[1] = {0};
/*	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //使能AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//先使能外设IO PORTA时钟	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	        // 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//根据设定参数初始化GPIOB
	
	MPU_AD0_CTRL = 0;                                   //控制MPU6050的AD0脚为低电平,从机地址为:0X68->0xD0	 直接接GND即可
*/
	buff[0] = 0x80;
	MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);	//复位MPU6050
	
	DelayMs(100);

	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);    //唤醒MPU6050
	
	MPU_Set_Gyro_Fsr(3);					          			//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);										//加速度传感器,±2g
	MPU_Set_Rate(50);						                    //设置采样率50Hz
	
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_INT_EN_REG,0x01,buff);	    //关闭所有中断
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_USER_CTRL_REG,0x01,buff);	//I2C主模式关闭
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_FIFO_EN_REG,0x01,buff);	    //关闭FIFO
	buff[0] = 0x80;
	MasterWriteDevice(MPU_ADDR,MPU_INTBP_CFG_REG,0x01,buff);	//INT引脚低电平有效 直接关闭中断
	
	MasterReadDevice(MPU_ADDR,MPU_DEVICE_ID_REG,0x01,res);     //读取从机地址

	if(res[0]==0x68)//器件ID正确
	{
		buff[0] = 0x01;
		MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);//设置CLKSEL,PLL X轴为参考
		buff[0] = 0x00;
		MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT2_REG,0x01,buff);//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						//设置采样率为50Hz
		printf("mpu6050 is OK!\r\n");
		printf("mpu6050 is OK!\r\n");
		printf("mpu6050 is OK!\r\n");
		return 0;
	}
	
	else
	{
		printf("mpu6050 is fail!\r\n");
		return 1;
	}
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	buff[0] = fsr<<3;
	return MasterWriteDevice(MPU_ADDR,MPU_GYRO_CFG_REG,0x01,buff);//设置陀螺仪满量程范围  
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败

u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	buff[0] = fsr<<3;
	return MasterWriteDevice(MPU_ADDR,MPU_ACCEL_CFG_REG,0x01,buff);//设置加速度传感器满量程范围  

}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6;

	buff[0] = data;
	return MasterWriteDevice(MPU_ADDR,MPU_CFG_REG,0x01,buff);//设置数字低通滤波器  
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)	rate=1000;
	if(rate<4)	rate=4;
	data=1000/rate-1;
	
	buff[0] = data;
	
	data=MasterWriteDevice(MPU_ADDR,MPU_SAMPLE_RATE_REG,0x01,buff);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}


