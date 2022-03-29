# V1.0

将C文件和头文件添加进工程；

修改mpu6050.h里面的IIC的句柄

```c
#define IIC hi2c2
```

使用MPU6050前先初始化

```c
MPU6050_Init();
```

函数说明：

```c
/**
  * return:成功返回ID，错误返回0
  */
uint8_t MPU6050ReadID(void);

/**
  * data：mpu6050数据结构体的指针
  * return:0成功，1失败
  */
uint8_t MPU6050_ReadAll(mpu6050 *data);
```

使用示例1:

```c
printf("ID: 0x%02x\r\n", MPU6050ReadID()); //正确应为0x68 失败为0x00
```

使用示例2：

```c
if(!MPU6050_ReadAll(&Mpu6050_Dat)) //Mpu6050_Dat变量已经在mpu6050.h声明不需要再定义
{
    /* MPU6050的温度值，已经处理好了 */
    printf("Temperature:%0.2f\r\n", Mpu6050_Dat.Temperature);
    /* xyz的角加速度原始值 */
    printf("gx:%d ", Mpu6050_Dat.gx);
    printf("gy:%d ", Mpu6050_Dat.gy);
    printf("gz:%d\r\n", Mpu6050_Dat.gz);
    /* xyz的加速度原始值 */
    printf("ax:%d ", Mpu6050_Dat.ax);
    printf("ay:%d ", Mpu6050_Dat.ay);
    printf("az:%d\r\n", Mpu6050_Dat.az);
}else printf("Read Failed\r\n");
```

# V2.0

改动：添加卡尔曼滤波、四元数计算

```c
 MPU6050_ReadAll(&Mpu6050_Dat);
 GetAngle(&Mpu6050_Dat, &Angle, Normal);
 printf("X: %0.2f  ", Angle.roll);//横滚角
 printf("Y: %0.2f  ", Angle.pitch);//俯仰角
 printf("Z: %0.2f\r\n", Angle.yaw);//偏航角
```

