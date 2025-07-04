#include "cs1237.h"
#include "delay.h"
#include "stdio.h"


#define CHANGE_ONCE     10
uint32_t cs1237_data[CHANGE_ONCE];

void CS1237_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = CS1237_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CS1237_SCK_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = CS1237_SDA_Pin;
    HAL_GPIO_Init(CS1237_SDA_GPIO_Port, &GPIO_InitStruct);
    
    CS1237_SCK_OUT(0);       
    CS1237_SDA_OUT(1); 
}

void SDA_SetInput()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = CS1237_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CS1237_SDA_GPIO_Port, &GPIO_InitStruct);
}

void SDA_SetOutput()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = CS1237_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(CS1237_SDA_GPIO_Port, &GPIO_InitStruct);
}

uint8_t CS1237_WaitReady()
{
    uint8_t count = 0;
    SDA_SetInput();     //输入模式
    delay_us(5);
    while(CS1237_SDA_READ) //等待数据准备好
    {
        delay_ms(3);
        count += 1;
        if(count > 100)
        {
            SDA_SetOutput();
            CS1237_SCK_OUT(0);
            CS1237_SDA_OUT(1);
#if USE_PRINTF_MSG
            printf("CS1237 wait ready for timeout!!!\n\r");
#endif
            return 1;
        }
    }
    
    delay_us(500);
    if(CS1237_SDA_READ)
        return 1;
    return 0;
}

void CS1237_WaitClock(uint16_t clk, uint8_t us)
{
    uint8_t i;
    for(i = 0; i < clk; i++) 
    {
        CS1237_SCK_OUT(1);
        delay_us(us);
        CS1237_SCK_OUT(0);
        delay_us(us);
    }
}

/* 30到36CLK 7bits配置模式 高位先发 */
void CS1237_RegMode(uint8_t cmd)
{
    uint8_t i,dat;
    dat = cmd;
    for(i = 0; i < 7; i++) 
    {
        dat <<= 1; 
        if(dat & 0x80)
            CS1237_SDA_OUT(1);
        else
            CS1237_SDA_OUT(0);
           
        CS1237_SCK_OUT(1);
        delay_us(5);
        CS1237_SCK_OUT(0);
        delay_us(5);
    }   
}


void CS1237_WriteConfig(uint8_t config)
{ 
    uint8_t i,dat;
    
    CS1237_SCK_OUT(0);  //拉低时钟
    delay_us(5);
    
    if(CS1237_WaitReady()) //等待寄存器数据准备
    {
        return;
    }
    
    //写配置忽略前29个时钟
    CS1237_WaitClock(29, 3);
    
    // 输出模式
    SDA_SetOutput();
    
    // 30到36CLK 写配置模式 高位先发 0x65 == b 0110 0101
    CS1237_RegMode(Write_CFG);
    
    // 37CLK 切换输出模式
    CS1237_WaitClock(1, 3);
    
    // 38到45CLK 发送配置数据 高位先发 
    dat = config;
    for(i = 38; i < 46; i++) 
    {
        if(dat & 0x80)
            CS1237_SDA_OUT(1);
        else
            CS1237_SDA_OUT(0);
        dat <<= 1; 
        
        CS1237_WaitClock(1, 3);
    }
    
    // 46CLK 切换输入模式，强制拉高
    CS1237_WaitClock(1, 3);
    
#if USE_PRINTF_MSG
    printf("CS1237 write config finshed!!!\n\r");
#endif
}

uint8_t CS1237_ReadConfig()
{
    uint8_t i,dat;
    
    CS1237_SCK_OUT(0);  //拉低时钟
    delay_us(5);
    if(CS1237_WaitReady())
    {
        return 0xff;
    }
    
    // 输出模式
    SDA_SetOutput();
    
    //写配置忽略前29个时钟
    CS1237_WaitClock(29, 3);
    
    // 30到36CLK 读配置模式 高位先发 0x56 == b 0101 0110
    CS1237_RegMode(Read_CFG);
    
    CS1237_SDA_OUT(1);

    // 37CLK 切换输入模式
    CS1237_WaitClock(1, 3);
    
    // 输入模式
    SDA_SetInput();
    
    // 38到45CLK 读取配置数据 高位先发
    dat = 0;
    for(i = 38; i < 46; i++) 
    {
        CS1237_WaitClock(1, 10);
        
        dat <<= 1; 
        if(CS1237_SDA_READ)
            dat += 1;
    }
    
    // 46CLK 切换输入模式，强制拉高
    CS1237_WaitClock(1, 3);
    SDA_SetOutput();   
    CS1237_SDA_OUT(1);

#if USE_PRINTF_MSG
    printf("CS1237 read config finshed!!!\n\r");
#endif    
    return dat;
}

uint32_t CS1237_ReadData()
{
    uint32_t data;
    uint8_t i;
    
    SDA_SetOutput();
    CS1237_SDA_OUT(1);  //端口锁存 1
    CS1237_SCK_OUT(0);  //拉低时钟
    delay_us(2);
    if(CS1237_WaitReady()) //等待数据准备
    {
        return 0;
    }
    
    // 输入模式
    SDA_SetInput();
    delay_us(8);
    data = 0;
    for(i = 0; i < 24; i++) 
    {
        CS1237_SCK_OUT(1);
        delay_us(4);
        
        data <<= 1; 
        if(CS1237_SDA_READ)
            data += 1;
            
        CS1237_SCK_OUT(0);
        delay_us(4);
    }
    
    CS1237_WaitClock(3, 3);
    
    SDA_SetOutput();
    CS1237_SDA_OUT(1); 
    delay_us(5);
    
    data = CS1237_DataAnalyzing(data);
    
    return data;
}


uint32_t CS1237_DataAnalyzing(uint32_t dat)
{
    uint32_t data = dat;
    uint8_t flag = 0; //正数的补码还是正数
    if((data & 0x800000) == 0x800000)	//最高位为1，表示输入为负值
	{
		data = ~data;  
		data += 1;
		data = data & 0xffffff;
		flag = 1;
#if USE_PRINTF_MSG
        printf("cs1237 get ad_value: -%u \n\r", data);
#endif 
    }
    else{
#if USE_PRINTF_MSG
        printf("cs1237 get ad_value: %u \n\r", data);
#endif     
    }
    flag_sign = flag;
    
    data >>= LOW_ACCURACY; //降低精度
    return data;
}

inline uint16_t CS1237_DataToWeight(uint32_t data, uint32_t data_zero)
{
    uint16_t weight = (data - data_zero) / CALIBRATION_FACTOR;
    return weight;
}

inline uint16_t CS1237_DataEmaWeight(uint16_t emaWeight, uint16_t curWeight)
{
    uint16_t weight = emaWeight * APHAL_FACTOR + curWeight * (1 - APHAL_FACTOR);
    return weight;
}

float CS1237_DataToVoltage(uint32_t data, uint8_t pga, float vref)
{
    float vol = 0;
    if(data < 1)
        return vol;
    
    // data * (0.5*vref/gain)/(2^23 -1)
    vol = data * (0.5 * vref / pga) / 8388608;
    if(flag_sign)
        vol = -(vol);
#if USE_PRINTF_MSG
        printf("cs1237 get Voltage: %.2f \n\r", vol);
#endif
    return vol;
}

void CS1237_ReadConfigOutput(uint8_t cfg)
{
#if USE_PRINTF_MSG
    //[7]   bits 默认写入0，不能写1
    if(cfg == 0xff) 
    {
        printf("cs1237 Read Config Error!!!\n\r");
        return;
    }
    
    uint8_t config;
    
    // [1:0] bits 通道选择
    config = cfg & CH_Int;
//    printf("cs1237 input channel select A: \n\r");
    switch(config)
    {
        case CH_A: printf("CH_A");break;
        case CH_Temp: printf("CH_Temp");break;
        case CH_Int: printf("CH_Int");break;
    }
    printf("\n\r");
    
    //[3:2] bits PGA 选择
    config = cfg & PGA_128;
//    printf("cs1237 selection of gain amplification factor: \n\r");
    switch(config)
    {
        case PGA_1: printf("PGA_1");break;
        case PGA_2: printf("PGA_2");break;
        case PGA_64: printf("PGA_64");break;
        case PGA_128: printf("PGA_128");break;
    }
    printf("\n\r");

    //[5:4] bits ADC 输出速率选择
    config = cfg & SpeedSelct_1280HZ;
//    printf("cs1237 ADC data output rate: \n\r");
    switch(config)
    {
        case SpeedSelct_10HZ: printf("SpeedSelct_10HZ");break;
        case SpeedSelct_40HZ: printf("SpeedSelct_40HZ");break;
        case SpeedSelct_640HZ: printf("SpeedSelct_640HZ");break;
        case SpeedSelct_1280HZ: printf("SpeedSelct_1280HZ");break;
    }
    printf("\n\r");
    
    //[6]   bits REF 输出开关
    config = cfg & RefOut_OFF;
//    printf("cs1237 REF output switch: \n\r");
    switch(config)
    {
        case RefOut_ON: printf("RefOut_ON");break;
        case RefOut_OFF: printf("RefOut_OFF");break;
    }
    printf("\n\r");
    
#endif     
}


