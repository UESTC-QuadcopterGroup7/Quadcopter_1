#include "stm32f4xx.h"
#include "Delay.h"
void MyI2C_W_SCL(uint8_t BitValue){
    GPIO_WriteBit(GPIOB,GPIO_Pin_1,(BitAction)BitValue);
    Delay_us(10);
}
void MyI2C_W_SDA(uint8_t BitValue){
    GPIO_WriteBit(GPIOB,GPIO_Pin_2,(BitAction)BitValue);
    Delay_us(10); 
}
uint8_t MyI2C_R_SDA(void){
    uint8_t BitValue;
    BitValue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2);
    Delay_us(10);
    return BitValue;
}
void MyI2C_Init(void){
    /*把SCL和SDA都初始化为开漏输出模式*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB,&GPIO_InitStructure);
    /*都置为高电平*/
    GPIO_SetBits(GPIOB,GPIO_Pin_1 | GPIO_Pin_2);
}

void MyI2C_Start(void){
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(0);

}


void MyI2C_Stop(void){
    MyI2C_W_SDA(0);       // SDA置低
    MyI2C_W_SCL(1);       // SCL置高（10us延时）
    Delay_us(5);          // 额外延时，确保SCL高电平稳定
    MyI2C_W_SDA(1);       // SDA拉高（停止信号）
}

void MyI2C_SendByte(uint8_t Byte){
    for(uint8_t i = 0; i < 8; i++){
        MyI2C_W_SDA( (Byte << i) & 0x80 );  
        MyI2C_W_SCL(1);  // 拉高SCL，让从机读取数据
        MyI2C_W_SCL(0);  // 拉低SCL，准备下一位
    }
}
uint8_t MyI2C_ReceiveByte(){
    uint8_t Byte = 0x00;
    MyI2C_W_SDA(1);  // 关键：读取前释放SDA，让传感器控制数据
    for(uint8_t i = 0;i < 8;i++){
        MyI2C_W_SCL(1);  // SCL拉高，传感器输出当前位数据
        Delay_us(5);     // 新增延时：等待传感器数据稳定（软件I2C必须加）
        if(MyI2C_R_SDA() == 1){
            Byte |= (0x80 >> i);  // 正确：从高位到低位读取
        }
        MyI2C_W_SCL(0);  // SCL拉低，准备下一位
        Delay_us(5);
    }
    return Byte;
}
void MyI2C_SendAck(uint8_t AckBit){
    MyI2C_W_SDA(AckBit);
    MyI2C_W_SCL(1);
    MyI2C_W_SCL(0); 
}
uint8_t MyI2C_ReceiveAck(){
    uint8_t AckBit = 0x00;
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);     
    AckBit = MyI2C_R_SDA();
    MyI2C_W_SCL(0); 
    return AckBit;
}
