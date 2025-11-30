#include "MySBUS.h"

#define SBUS_BAUDRATE     100000
#define SBUS_START_BYTE   0x0F
#define SBUS_END_BYTE     0x00

static volatile SBUS_Data_t sbusData;
static uint8_t sbusBuf[SBUS_FRAME_LEN];
static volatile uint8_t sbusIndex = 0;

static void SBUS_ParseFrame(const uint8_t *buf)
{
    sbusData.channels[0]  = (buf[1]      | buf[2]  << 8) & 0x07FF;
    sbusData.channels[1]  = (buf[2] >> 3 | buf[3]  << 5) & 0x07FF;
    sbusData.channels[2]  = (buf[3] >> 6 | buf[4]  << 2 | buf[5] << 10) & 0x07FF;
    sbusData.channels[3]  = (buf[5] >> 1 | buf[6]  << 7) & 0x07FF;
    sbusData.channels[4]  = (buf[6] >> 4 | buf[7]  << 4) & 0x07FF;
    sbusData.channels[5]  = (buf[7] >> 7 | buf[8]  << 1 | buf[9] << 9) & 0x07FF;
    sbusData.channels[6]  = (buf[9] >> 2 | buf[10] << 6) & 0x07FF;
    sbusData.channels[7]  = (buf[10]>>5 | buf[11] << 3) & 0x07FF;
    sbusData.channels[8]  = (buf[12]     | buf[13] << 8) & 0x07FF;
    sbusData.channels[9]  = (buf[13]>>3  | buf[14] << 5) & 0x07FF;
    sbusData.channels[10] = (buf[14]>>6  | buf[15] << 2 | buf[16] << 10) & 0x07FF;
    sbusData.channels[11] = (buf[16]>>1  | buf[17] << 7) & 0x07FF;
    sbusData.channels[12] = (buf[17]>>4  | buf[18] << 4) & 0x07FF;
    sbusData.channels[13] = (buf[18]>>7  | buf[19] << 1 | buf[20] << 9) & 0x07FF;
    sbusData.channels[14] = (buf[20]>>2  | buf[21] << 6) & 0x07FF;
    sbusData.channels[15] = (buf[21]>>5  | buf[22] << 3) & 0x07FF;

    sbusData.frameLost = (buf[23] & 0x04) ? 1 : 0;
    sbusData.failsafe  = (buf[23] & 0x08) ? 1 : 0;
    sbusData.updated   = 1;
}

static void SBUS_GPIO_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_11; // PB11 -> USART3_RX
    gpio.GPIO_Mode  = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART2);
}

static void SBUS_USART_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitTypeDef usart;
    USART_StructInit(&usart);
    usart.USART_BaudRate            = SBUS_BAUDRATE;
    usart.USART_WordLength          = USART_WordLength_8b;
    usart.USART_StopBits            = USART_StopBits_2;
    usart.USART_Parity              = USART_Parity_Even;
    usart.USART_Mode                = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = USART2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_Cmd(USART2, ENABLE);
}

void SBUS_Init(void)
{
    sbusIndex = 0;
    sbusData.updated = 0;
    SBUS_GPIO_Init();
    SBUS_USART_Init();
}

const SBUS_Data_t *SBUS_GetData(void)
{
    sbusData.updated = 0;
    return (const SBUS_Data_t *)&sbusData;
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint8_t byte = (uint8_t)USART_ReceiveData(USART2);

        if (sbusIndex == 0)
        {
            if (byte == SBUS_START_BYTE)
            {
                sbusBuf[sbusIndex++] = byte;
            }
        }
        else
        {
            sbusBuf[sbusIndex++] = byte;
            if (sbusIndex >= SBUS_FRAME_LEN)
            {
                if (sbusBuf[0] == SBUS_START_BYTE &&
                    (byte == SBUS_END_BYTE || byte == 0x04 || byte == 0x07))
                {
                    SBUS_ParseFrame(sbusBuf);
                }
                sbusIndex = 0;
            }
        }
    }
}
