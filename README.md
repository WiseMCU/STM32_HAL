# V1.0

BH1750光照传感器

```c
 uint8_t dat[2] = {0};
 BH1750_Send_Cmd(ONCE_H_MODE);//设置工作模式
 HAL_Delay(200);//等待转换
 BH1750_Read_Dat(dat);//获取原始数据
 printf("current: %5d lux\r\n", BH1750_Dat_To_Lux(dat));//显示光强
```

