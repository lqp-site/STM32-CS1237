#ifndef __CS1237_H
#define __CS1237_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define CALIBRATION_FACTOR      200 //ADCת��������У׼����
#define APHAL_FACTOR            0.2 //EMA�˲�ϵ��
#define LOW_ACCURACY            4   //ADCת�����;��ȣ�����λ�� 

/* 7 bits ��������*/
#define Write_CFG           0x65    //д���á�
#define Read_CFG            0x56    //�����á�

/* �Ĵ���������
* [1:0] bits ͨ��ѡ��
* [3:2] bits PGA ѡ��
* [5:4] bits ADC �������ѡ��
* [6]   bits REF �������
* [7]   bits Ĭ��д��0������д1
*/

#define RefOut_ON           0x00    //REF ���������
#define RefOut_OFF          0x40    //�ر� REF �����

#define SpeedSelct_10HZ     0x00    //ADC �������Ϊ 10Hz
#define SpeedSelct_40HZ     0x10    //ADC �������Ϊ 40Hz
#define SpeedSelct_640HZ    0x20    //ADC �������Ϊ 6400Hz
#define SpeedSelct_1280HZ   0x30    //ADC �������Ϊ 12800Hz
                            
#define PGA_1               0x00    //�Ŵ�λ��ѡ��1
#define PGA_2               0x04    //�Ŵ�λ��ѡ��2
#define PGA_64              0x08    //�Ŵ�λ��ѡ��64
#define PGA_128             0x0C    //�Ŵ�λ��ѡ��128
                            
#define CH_A                0x00    //����ͨ��ѡ��A
#define CH_Temp             0x02    //����ͨ��ѡ���ڲ��¶Ȳ���
#define CH_Int              0x03    //����ͨ��ѡ���ڲ���·


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

