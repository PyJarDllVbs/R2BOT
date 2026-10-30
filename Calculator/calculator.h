#include <stdint.h>
#include <stdlib.h>
/*              ^ Vy
     轮0        |           轮1
                |
                |
                | Vw顺时针为正
---------------------------------->Vx
                | 
                |
     轮2        |           轮3
                |
*/
#define deadzone 0.05
#define maxccr 7200
#define step 10
#define maxsteer 2000
#define minsteer 500
typedef struct {
    int16_t Vx,Vy,Vw;//分别代表横向、纵向、自传移动速度的模拟量
    int8_t screw[2];//
    int8_t claw[2];
    uint8_t brake;
}QuantifyHandleDef;//中间量合集，转录PS2操作指令，作为计算的数据来源
typedef struct{
    int16_t wheel[4];
    int16_t claw[2];
    int16_t screw[2];
    uint8_t brake;
}PwmHandleDef;
extern volatile QuantifyHandleDef quantify;
extern volatile PwmHandleDef pwm;
void calculate(void);

