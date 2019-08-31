#include "mpu6050.h"

unsigned char buff[1] = {0};
/*****************************************************************************
*
*  ����:   MPU_Init
*
*  ����:   ����ֵ 0  �ɹ�
*
*          ����ֵ���������
*****************************************************************************/
u8 MPU_Init(void)
{
	u8 res[1] = {0};
/*	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ��AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//��ʹ������IO PORTAʱ��	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	        // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//�����趨������ʼ��GPIOB
	
	MPU_AD0_CTRL = 0;                                   //����MPU6050��AD0��Ϊ�͵�ƽ,�ӻ���ַΪ:0X68->0xD0	 ֱ�ӽ�GND����
*/
	buff[0] = 0x80;
	MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);	//��λMPU6050
	
	DelayMs(100);

	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);    //����MPU6050
	
	MPU_Set_Gyro_Fsr(3);					          			//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);										//���ٶȴ�����,��2g
	MPU_Set_Rate(50);						                    //���ò�����50Hz
	
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_INT_EN_REG,0x01,buff);	    //�ر������ж�
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_USER_CTRL_REG,0x01,buff);	//I2C��ģʽ�ر�
	buff[0] = 0x00;
	MasterWriteDevice(MPU_ADDR,MPU_FIFO_EN_REG,0x01,buff);	    //�ر�FIFO
	buff[0] = 0x80;
	MasterWriteDevice(MPU_ADDR,MPU_INTBP_CFG_REG,0x01,buff);	//INT���ŵ͵�ƽ��Ч ֱ�ӹر��ж�
	
	MasterReadDevice(MPU_ADDR,MPU_DEVICE_ID_REG,0x01,res);     //��ȡ�ӻ���ַ

	if(res[0]==0x68)//����ID��ȷ
	{
		buff[0] = 0x01;
		MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT1_REG,0x01,buff);//����CLKSEL,PLL X��Ϊ�ο�
		buff[0] = 0x00;
		MasterWriteDevice(MPU_ADDR,MPU_PWR_MGMT2_REG,0x01,buff);//���ٶ��������Ƕ�����
		MPU_Set_Rate(50);						//���ò�����Ϊ50Hz
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

//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	buff[0] = fsr<<3;
	return MasterWriteDevice(MPU_ADDR,MPU_GYRO_CFG_REG,0x01,buff);//���������������̷�Χ  
}

//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ��

u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	buff[0] = fsr<<3;
	return MasterWriteDevice(MPU_ADDR,MPU_ACCEL_CFG_REG,0x01,buff);//���ü��ٶȴ����������̷�Χ  

}

//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
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
	return MasterWriteDevice(MPU_ADDR,MPU_CFG_REG,0x01,buff);//�������ֵ�ͨ�˲���  
}

//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)	rate=1000;
	if(rate<4)	rate=4;
	data=1000/rate-1;
	
	buff[0] = data;
	
	data=MasterWriteDevice(MPU_ADDR,MPU_SAMPLE_RATE_REG,0x01,buff);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}


