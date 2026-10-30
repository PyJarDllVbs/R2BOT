#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "PS2.h"
#include "core_cm3.h"
volatile PS2_HandleTypeDef ps2;
volatile uint8_t Data[9];
void PS2_Init(void)//初始化PS2对应引脚
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  HAL_GPIO_WritePin(PS2_CS_GPIO_Port, PS2_CS_Pin|PS2_CLK_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PS2_CMD_GPIO_Port, PS2_CMD_Pin, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = PS2_CS_Pin|PS2_CLK_Pin|PS2_CMD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PS2_CS_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PS2_DAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PS2_DAT_GPIO_Port, &GPIO_InitStruct);
	HAL_Delay(100);
}
static void delay_us(uint32_t us) //延时函数，单位微秒
{
	uint32_t wait = us*(SystemCoreClock/1000000);//计算需要等待的时钟数
	wait/=3;
	for(;wait>0;wait--){__NOP();}//空转等待
}
static uint8_t PS2_Transbyte(uint8_t cmd)//交换字节，输入发送的8位整数，接受PS2发回的并返回
{
	uint8_t data=0;
	for(int i=0;i<8;i++)
	{
		if(cmd&(1<<i)) HAL_GPIO_WritePin(PS2_CMD_GPIO_Port,PS2_CMD_Pin,GPIO_PIN_SET);
		else HAL_GPIO_WritePin(PS2_CMD_GPIO_Port,PS2_CMD_Pin,GPIO_PIN_RESET);

		HAL_GPIO_WritePin(PS2_CLK_GPIO_Port,PS2_CLK_Pin,GPIO_PIN_RESET);
		delay_us(5);
		if(HAL_GPIO_ReadPin(PS2_DAT_GPIO_Port,PS2_DAT_Pin)==GPIO_PIN_SET) data |=(1<<i);

		HAL_GPIO_WritePin(PS2_CLK_GPIO_Port,PS2_CLK_Pin,GPIO_PIN_SET);
		delay_us(5);
	}
	HAL_GPIO_WritePin(PS2_CMD_GPIO_Port,PS2_CMD_Pin,GPIO_PIN_SET);
	delay_us(4);
	return data;
}
static void PS2_ReadData(void)
{
	HAL_GPIO_WritePin(PS2_CS_GPIO_Port,PS2_CS_Pin,GPIO_PIN_RESET);
	delay_us(100);
	Data[8]=PS2_Transbyte(0x01);//起始命令，手柄回复0xFF空闲
	Data[0]=PS2_Transbyte(0x42);//询问红灯绿灯模式，手柄回复0x41绿灯模式，0x73红灯模式
	Data[7]=PS2_Transbyte(0x00);//数据即将到来，手柄回复0x5A
	Data[1]=PS2_Transbyte(0x00);//按键数据1（SELECT、L3、R3、START、方向键）
	Data[2]=PS2_Transbyte(0x00);//按键数据2（L2, R2, L1, R1, △, ○, ✕, □）
	Data[3]=PS2_Transbyte(0x00);//右摇杆x轴
	Data[4]=PS2_Transbyte(0x00);//右遥杆y轴
	Data[5]=PS2_Transbyte(0x00);//左摇杆x轴
	Data[6]=PS2_Transbyte(0x00);//左遥杆y轴
	HAL_GPIO_WritePin(PS2_CLK_GPIO_Port,PS2_CLK_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(PS2_CMD_GPIO_Port,PS2_CMD_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(PS2_CS_GPIO_Port,PS2_CS_Pin,GPIO_PIN_SET);
	delay_us(500);
}
static void reservedata(void)
{
	ps2.mode=Data[0];
	ps2.keys[0]=Data[1];
	ps2.keys[1]=Data[2];
	ps2.rx=Data[3];
	ps2.ry=Data[4];
	ps2.lx=Data[5];
	ps2.ly=Data[6];
	ps2.connected=(uint8_t)((Data[8]==0xFF)&&(Data[0]==0x73)&&(Data[7]==0x5A));
}
void PS2_Update(void)
{
	PS2_ReadData();
	reservedata();
}
