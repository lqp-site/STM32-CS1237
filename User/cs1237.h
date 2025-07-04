#ifndef __CS1237_H
#define __CS1237_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define CALIBRATION_FACTOR      200 //ADC转换重量的校准因子
#define APHAL_FACTOR            0.2 //EMA滤波系数
#define LOW_ACCURACY            4   //ADC转换降低精度，右移位数 

/* 7 bits 的命令字*/
#define Write_CFG           0x65    //写配置。
#define Read_CFG            0x56    //读配置。

/* 寄存器配置项
* [1:0] bits 通道选择
* [3:2] bits PGA 选择
* [5:4] bits ADC 输出速率选择
* [6]   bits REF 输出开关
* [7]   bits 默认写入0，不能写1
*/

#define RefOut_ON           0x00    //REF 正常输出。
#define RefOut_OFF          0x40    //关闭 REF 输出。

#define SpeedSelct_10HZ     0x00    //ADC 输出速率为 10Hz
#define SpeedSelct_40HZ     0x10    //ADC 输出速率为 40Hz
#define SpeedSelct_640HZ    0x20    //ADC 输出速率为 6400Hz
#define SpeedSelct_1280HZ   0x30    //ADC 输出速率为 12800Hz
                            
#define PGA_1               0x00    //放大位数选择1
#define PGA_2               0x04    //放大位数选择2
#define PGA_64              0x08    //放大位数选择64
#define PGA_128             0x0C    //放大位数选择128
                            
#define CH_A                0x00    //输入通道选择A
#define CH_Temp             0x02    //输入通道选择内部温度测试
#define CH_Int              0x03    //输入通道选择内部短路


void CS1237_init(void);
void SDA_SetInput(void);
void SDA_SetOutput(void);
uint8_t CS1237_WaitReady(void);
void CS1237_WaitClock(uint16_t clk, uint8_t us);
void CS1237_RegMode(uint8_t cmd);
void CS1237_WriteConfig(uint8_t config);
uint8_t CS1237_ReadConfig(void);
uint32_t CS1237_ReadData(void);

uint32_t CS1237_DataAnalyzing(uint32_t data);
uint16_t CS1237_DataToWeight(uint32_t data, uint32_t data_zero);
uint16_t CS1237_DataEmaWeight(uint16_t emaWeight, uint16_t curWeight);
float CS1237_DataToVoltage(uint32_t data, uint8_t pga, float vref);
void CS1237_ReadConfigOutput(uint8_t cfg);

#ifdef __cplusplus
}
#endif

#endif /* __CS1237_H */

