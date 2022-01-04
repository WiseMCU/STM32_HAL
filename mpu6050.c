#include "mpu6050.h"
mpu6050 Mpu6050_Dat = {0};
/**
  * @brief   写数据到MPU6050寄存器
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
  * @brief   从MPU6050寄存器读取数据
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
  * @brief   初始化MPU6050芯片
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  HAL_Delay(50);
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	     //解除休眠状态
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x01);	  //配置加速度传感器工作在2G模式
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}

/**
  * @brief   读取MPU6050的ID
  * @param   
  * @retval  正常返回ID，异常返回0
  */
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
  MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
	return Re;
}

/**
  * @brief   读取MPU6050的加速度数据
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
  * @brief   读取MPU6050的角加速度数据
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
  * @brief   读取MPU6050的温度数据，转化成摄氏度
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
  * @brief   读取MPU6050的主要数据,温度经过处理，其他是原始参数
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
