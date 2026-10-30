#include "calculator.h"
#include "PS2.h"
volatile uint8_t maxdire;
volatile QuantifyHandleDef quantify;
volatile PwmHandleDef pwm;
void takenotes(void)
{
    if(ps2.connected==0)
    {
        quantify.Vx=0;quantify.Vy=0;quantify.Vw=0;
        quantify.screw[0]=0;quantify.screw[1]=0;
        quantify.claw[0]=0;quantify.claw[1]=0;
        quantify.brake=0;
        return;

    }//手柄未连接则不执行
    //抄录PS2摇杆加死区判断换向
    maxdire=0;
    quantify.Vx=-128+ps2.lx;
    if(abs(quantify.Vx)<128*deadzone){quantify.Vx=0;}
    if(abs(quantify.Vx)>maxdire){maxdire=abs(quantify.Vx);}
    
    quantify.Vy=128-ps2.ly;
    if(abs(quantify.Vy)<128*deadzone){quantify.Vy=0;}
    if(abs(quantify.Vy)>maxdire){maxdire=abs(quantify.Vy);}
    
    quantify.Vw=-128+ps2.rx;
    if(abs(quantify.Vw)<128*deadzone){quantify.Vw=0;}
    if(abs(quantify.Vw)>maxdire){maxdire=abs(quantify.Vw);}
    //抄录PS2丝杠指令（1上升，0不动，-1下降）
    uint8_t up_pressed=!(ps2.keys[0]&(1<<4));
    uint8_t down_pressed=!(ps2.keys[0]&(1<<6));
    quantify.screw[0]=up_pressed-down_pressed;
    up_pressed=!(ps2.keys[1]&(1<<4));
    down_pressed=!(ps2.keys[1]&(1<<6));
    quantify.screw[1]=up_pressed-down_pressed;
    //抄录PS2夹爪指令（1闭合，0不动，-1张开）
    uint8_t in_pressed=!(ps2.keys[1]&(1<<0));
    uint8_t out_pressed=!(ps2.keys[1]&(1<<2));
    quantify.claw[0]=in_pressed-out_pressed;
    in_pressed=!(ps2.keys[1]&(1<<1));
    out_pressed=!(ps2.keys[1]&(1<<3));
    quantify.claw[1]=in_pressed-out_pressed;
    //抄录刹车指令
    quantify.brake=!(ps2.keys[1]&(1<<7));
}
void compute(void)
{
    uint8_t maxwheel=0;
    pwm.wheel[0]=quantify.Vy+quantify.Vx+quantify.Vw;
    if(abs(pwm.wheel[0])>abs(pwm.wheel[maxwheel])){maxwheel=0;}
    pwm.wheel[1]=quantify.Vy-quantify.Vx-quantify.Vw;
    if(abs(pwm.wheel[1])>abs(pwm.wheel[maxwheel])){maxwheel=1;}
    pwm.wheel[2]=quantify.Vy-quantify.Vx+quantify.Vw;
    if(abs(pwm.wheel[2])>abs(pwm.wheel[maxwheel])){maxwheel=2;}
    pwm.wheel[3]=quantify.Vy+quantify.Vx-quantify.Vw;
    if(abs(pwm.wheel[3])>abs(pwm.wheel[maxwheel])){maxwheel=3;}
    if(pwm.wheel[maxwheel]!=0)
    {
        uint32_t model=maxdire/128.0f*maxccr/abs(pwm.wheel[maxwheel]);
        for(int i=0;i<4;i++)
        {
            pwm.wheel[i]=pwm.wheel[i]*model;
        }
    }
    pwm.screw[0]=quantify.screw[0]*maxccr;
    pwm.screw[1]=quantify.screw[1]*maxccr;
    pwm.claw[0]+=quantify.claw[0]*step;
    pwm.claw[1]+=quantify.claw[1]*step;
    if(pwm.claw[0]>maxsteer){pwm.claw[0]=maxsteer;}
    if(pwm.claw[1]>maxsteer){pwm.claw[1]=maxsteer;}
    if(pwm.claw[0]<minsteer){pwm.claw[0]=minsteer;}
    if(pwm.claw[1]<minsteer){pwm.claw[1]=minsteer;}
    pwm.brake=quantify.brake;
}
void calculate(void)
{
	takenotes();
	compute();


}
