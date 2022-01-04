#include "mpu6050.h"
mpu6050 Mpu6050_Dat = {0};
/**
  * @brief   д���ݵ�MPU6050�Ĵ���
  * @param   
  * @retval  
  */
uint8_t MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
	if(HAL_I2C_Mem_Write(&IIC, 0XD0, reg_add, I2C_MEMADD_SIZE_8BIT, &reg_dat, 1, 0xff) == HAL_OK)
	{
		HAL_Delay(1);
		return SUCCESS;
	}
	return ERROR;
}

/**
  * @brief   ��MPU6050�Ĵ�����ȡ����
  * @param   
  * @retval  
  */
uint8_t MPU6050_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num)
{
	if(HAL_I2C_Mem_Read(&IIC, 0XD1, reg_add, I2C_MEMADD_SIZE_8BIT, Read, num, 0xff) == HAL_OK)
	{
		HAL_Delay(1);
		return SUCCESS;
	}
	return ERROR;
}


/**
  * @brief   ��ʼ��MPU6050оƬ
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
  //�ڳ�ʼ��֮ǰҪ��ʱһ��ʱ�䣬��û����ʱ����ϵ�����ϵ����ݿ��ܻ����
  HAL_Delay(50);
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	     //�������״̬
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //�����ǲ�����
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x01);	  //���ü��ٶȴ�����������2Gģʽ
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
}

/**
  * @brief   ��ȡMPU6050��ID
  * @param   
  * @retval  ��������ID���쳣����0
  */
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
  MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //��������ַ
	return Re;
}

/**
  * @brief   ��ȡMPU6050�ļ��ٶ�����
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadAcc(mpu6050 *data)
{
	uint8_t buf[6];
	if(!MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6))
	{
		data->ax = (short)(buf[0] << 8) | buf[1];
		data->ay = (short)(buf[2] << 8) | buf[3];
		data->az = (short)(buf[4] << 8) | buf[5];
		return SUCCESS;
	}
	return ERROR;
}

/**
  * @brief   ��ȡMPU6050�ĽǼ��ٶ�����
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadGyro(mpu6050 *data)
{
  uint8_t buf[6];
	if(!MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6))
	{
		data->gx = (short)(buf[0] << 8) | buf[1];
    data->gy = (short)(buf[2] << 8) | buf[3];
    data->gz = (short)(buf[4] << 8) | buf[5];
		return SUCCESS;
	}
  return ERROR;
}

/**
  * @brief   ��ȡMPU6050���¶����ݣ�ת�������϶�
  * @param   
  * @retval  
  */
uint8_t MPU6050_ReturnTemp(mpu6050 *data)
{
	short temp3;
	uint8_t buf[2];
	if(!MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2))
	{
		temp3= (buf[0] << 8) | buf[1];	
		data->Temperature =((double) temp3/340.0)+36.53;
		return SUCCESS;
	}
  return ERROR;
}
/**
  * @brief   ��ȡMPU6050����Ҫ����,�¶Ⱦ�������������ԭʼ����
  * @param   
  * @retval  
  */
uint8_t MPU6050_ReadAll(mpu6050 *data)
{
	mpu6050 buf = {0};
	if(!(MPU6050_ReturnTemp(&buf) | MPU6050ReadGyro(&buf) | MPU6050ReadAcc(&buf)))
	{
		*data = buf;
		return SUCCESS;
	}
	return ERROR;
}
