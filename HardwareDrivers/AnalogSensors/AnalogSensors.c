#include "AnalogSensors.h"

#define HEARTRATE_PORT        GPIOC
#define HEARTRATE_PIN         GPIO_Pin_2
#define HEARTRATE_ADC_CHANNEL ADC_Channel_12
#define LIGHT_PORT            GPIOC
#define LIGHT_PIN             GPIO_Pin_3
#define LIGHT_ADC_CHANNEL     ADC_Channel_13

#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)

volatile uint16_t AnalogSensors_ADCValue;
//__IO u16 ADC_ConvertedValueLocal;


void AnalogSensors_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = HEARTRATE_PIN | LIGHT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC1,输入时不用设置速率
}



void AnalogSensors_ADCMode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AnalogSensors_ADCValue;//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;  //内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 configuration */

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 //禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	 	//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);

	/*配置ADC时钟，为PCLK2的8分频，即9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	/*配置ADC1的通道11为55.	5个采样周期，序列为1 */
	ADC_RegularChannelConfig(ADC1, LIGHT_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/*复位校准寄存器 */
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));

	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void AnalogSensors_Light_Init(void)
{
	AnalogSensors_GPIO_Config();
	AnalogSensors_ADCMode_Config();
	ADC_RegularChannelConfig(ADC1, LIGHT_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
}

void AnalogSensors_HeartRate_Init(void)
{
	AnalogSensors_GPIO_Config();
	AnalogSensors_ADCMode_Config();
	ADC_RegularChannelConfig(ADC1, HEARTRATE_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
}

void AnalogSensors_Light_DeInit(void)
{
	ADC_DeInit(ADC1);
	DMA_DeInit(DMA1_Channel1);
}

void AnalogSensors_HeartRate_DeInit(void)
{
	ADC_DeInit(ADC1);
	DMA_DeInit(DMA1_Channel1);
}
