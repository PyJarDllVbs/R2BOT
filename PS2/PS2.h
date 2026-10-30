//这是PS2通信驱动的头文件，只需incliude便可在main调用
//定义引脚名
#include "stm32f1xx_hal.h"
#include <stdint.h>
#define PS2_CS_Pin GPIO_PIN_8
#define PS2_CS_GPIO_Port GPIOB
#define PS2_CLK_Pin GPIO_PIN_9
#define PS2_CLK_GPIO_Port GPIOB
#define PS2_DAT_Pin GPIO_PIN_13
#define PS2_DAT_GPIO_Port GPIOB
#define PS2_CMD_Pin GPIO_PIN_14
#define PS2_CMD_GPIO_Port GPIOB
//定义结构体变量
typedef struct{
	uint8_t keys[2];
	uint8_t  lx,ly,rx,ry;
	uint8_t mode;
	uint8_t connected;
} PS2_HandleTypeDef;
extern volatile PS2_HandleTypeDef ps2;
//可调用接口声明
void PS2_Init(void);//初始化PS2对应引脚
void PS2_Update(void);//读取PS2状态并更新ps2结构体
