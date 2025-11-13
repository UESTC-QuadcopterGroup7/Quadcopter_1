# STM32F401RET6 引脚映射

| 原生引脚 | 类型 | 主功能       |                    复用功能（含 AF 编号）                    | 开发板标识（NUCLEO-F401RE） |
| -------- | ---- | ------------ | :----------------------------------------------------------: | --------------------------- |
| PA0      | I/O  | PA0          | USART2_CTS (AF7)、TIM2_CH1 (AF1)、TIM5_CH1 (AF2)、TIM2_ETR (AF1)、EVENT OUT (AF15) | A0（模拟 / 数字复用）       |
| PA1      | I/O  | PA1          | USART2_RTS (AF7)、TIM2_CH2 (AF1)、TIM5_CH2 (AF2)、TIM2_ETR (AF1)、EVENT OUT (AF15) | A1（模拟 / 数字复用）       |
| PA2      | I/O  | PA2          | USART2_TX (AF7)、TIM2_CH3 (AF1)、TIM5_CH3 (AF2)、TIM9_CH1 (AF3)、EVENT OUT (AF15) | D2（数字输出 / USART2_TX）  |
| PA3      | I/O  | PA3          | USART2_RX (AF7)、TIM2_CH4 (AF1)、TIM5_CH4 (AF2)、TIM9_CH2 (AF3)、EVENT OUT (AF15) | D0（数字输入 / USART2_RX）  |
| PA4      | I/O  | PA4          | SPI1_NSS (AF5)、SPI3_NSS/I2S3_WS (AF6)、USART2_CK (AF7)、EVENT OUT (AF15) | A2（模拟 / DAC1）           |
| PA5      | I/O  | PA5          |  SPI1_SCK (AF5)、TIM2_CH1/TIM2_ETR (AF1)、EVENT OUT (AF15)   | D13（数字 / SPI1_SCK）      |
| PA6      | I/O  | PA6          | SPI1_MISO (AF5)、TIM1_BKIN (AF1)、TIM3_CH1 (AF2)、EVENT OUT (AF15) | D12（数字 / SPI1_MISO）     |
| PA7      | I/O  | PA7          | SPI1_MOSI (AF5)、TIM1_CH1N (AF1)、TIM3_CH2 (AF2)、EVENT OUT (AF15) | D11（数字 / SPI1_MOSI）     |
| PA8      | I/O  | PA8          | I2C3_SCL (AF4)、USART1_CK (AF7)、TIM1_CH1 (AF1)、OTG_FS_SOF (AF10)、MCO1 (AF0)、EVENT OUT (AF15) | D7（数字）                  |
| PA9      | I/O  | PA9          | I2C3_SMBA (AF4)、USART1_TX (AF7)、TIM1_CH2 (AF1)、EVENT OUT (AF15) | D8（数字）                  |
| PA10     | I/O  | PA10         | USART1_RX (AF7)、TIM1_CH3 (AF1)、OTG_FS_ID (AF10)、EVENT OUT (AF15) | D9（数字）                  |
| PA11     | I/O  | PA11         | USART1_CTS (AF7)、USART6_TX (AF7)、TIM1_CH4 (AF1)、OTG_FS_DM (AF10)、EVENT OUT (AF15) | D10（数字 / USB_OTG_FS_DM） |
| PA12     | I/O  | PA12         | USART1_RTS (AF7)、USART6_RX (AF7)、TIM1_ETR (AF1)、OTG_FS_DP (AF10)、EVENT OUT (AF15) | -（扩展引脚）               |
| PA13     | I/O  | PA13         |              JTMS/SWDIO (AF0)、EVENT OUT (AF15)              | -（调试接口）               |
| PA14     | I/O  | PA14         |              JTCK/SWCLK (AF0)、EVENT OUT (AF15)              | -（调试接口）               |
| PA15     | I/O  | PA15         | JTDI (AF0)、SPI1_NSS (AF5)、SPI3_NSS/I2S3_WS (AF6)、TIM2_CH1/TIM2_ETR (AF1)、EVENT OUT (AF15) | -（扩展引脚）               |
| PB0      | I/O  | PB0          |      TIM1_CH2N (AF1)、TIM3_CH3 (AF2)、EVENT OUT (AF15)       | A3（模拟 / DAC2）           |
| PB1      | I/O  | PB1          |      TIM1_CH3N (AF1)、TIM3_CH4 (AF2)、EVENT OUT (AF15)       | A4（模拟）                  |
| PB2      | I/O  | PB2          |                       EVENT OUT (AF15)                       | -（BOOT 配置）              |
| PB3      | I/O  | PB3          | JTDO/TRACESWO (AF0)、SPI1_SCK (AF5)、SPI3_SCK/I2S3_CK (AF6)、I2C2_SDA (AF4)、TIM2_CH2 (AF1)、EVENT OUT (AF15) | D9（数字）                  |
| PB4      | I/O  | PB4          | NJTRST (AF0)、SPI1_MISO (AF5)、SPI3_MISO (AF6)、I2C3_SDA (AF4)、TIM3_CH1 (AF2)、EVENT OUT (AF15) | A5（模拟）                  |
| PB5      | I/O  | PB5          | SPI1_MOSI (AF5)、SPI3_MOSI/I2S3_SD (AF6)、I2C1_SMBA (AF4)、TIM3_CH2 (AF2)、EVENT OUT (AF15) | -（扩展引脚）               |
| PB6      | I/O  | PB6          | I2C1_SCL (AF4)、USART1_TX (AF7)、TIM4_CH1 (AF2)、EVENT OUT (AF15) | -（扩展引脚）               |
| PB7      | I/O  | PB7          | I2C1_SDA (AF4)、USART1_RX (AF7)、TIM4_CH2 (AF2)、EVENT OUT (AF15) | -（扩展引脚）               |
| PB8      | I/O  | PB8          | I2C1_SCL (AF4)、TIM4_CH3 (AF2)、TIM10_CH1 (AF3)、SDIO_D4 (AF13)、EVENT OUT (AF15) | D4（数字）                  |
| PB9      | I/O  | PB9          | SPI2_NSS/I2S2_WS (AF10)、I2C1_SDA (AF4)、TIM4_CH4 (AF2)、TIM11_CH1 (AF3)、SDIO_D5 (AF13)、EVENT OUT (AF15) | D5（数字）                  |
| PB10     | I/O  | PB10         | SPI2_SCK/I2S2_CK (AF10)、I2C2_SCL (AF4)、TIM2_CH3 (AF1)、EVENT OUT (AF15) | D6（数字）                  |
| PB11     | I/O  | PB11         |                       EVENT OUT (AF15)                       | -（扩展引脚）               |
| PB12     | I/O  | PB12         | SPI2_NSS/I2S2_WS (AF10)、I2C2_SMBA (AF4)、TIM1_BKIN (AF1)、EVENT OUT (AF15) | -（扩展引脚）               |
| PB13     | I/O  | PB13         |  SPI2_SCK/I2S2_CK (AF10)、TIM1_CH1N (AF1)、EVENT OUT (AF15)  | -（扩展引脚）               |
| PB14     | I/O  | PB14         | SPI2_MISO (AF10)、I2S2EXT_SD (AF10)、TIM1_CH2N (AF1)、EVENT OUT (AF15) | D15（数字）                 |
| PB15     | I/O  | PB15         | SPI2_MOSI/I2S2_SD (AF10)、TIM1_CH3N (AF1)、EVENT OUT (AF15)  | D14（数字）                 |
| PC0      | I/O  | PC0          |               TIM3_CH1 (AF2)、EVENT OUT (AF15)               | A6（模拟）                  |
| PC1      | I/O  | PC1          |               TIM3_CH2 (AF2)、EVENT OUT (AF15)               | A7（模拟）                  |
| PC2      | I/O  | PC2          |    SPI2_MISO (AF10)、I2S2EXT_SD (AF10)、EVENT OUT (AF15)     | A8（模拟）                  |
| PC3      | I/O  | PC3          |          SPI2_MOSI/I2S2_SD (AF10)、EVENT OUT (AF15)          | A9（模拟）                  |
| PC4      | I/O  | PC4          |                       EVENT OUT (AF15)                       | A10（模拟）                 |
| PC5      | I/O  | PC5          |                       EVENT OUT (AF15)                       | A11（模拟）                 |
| PC6      | I/O  | PC6          | I2S2_MCK (AF10)、USART6_TX (AF7)、TIM3_CH1 (AF2)、SDIO_D6 (AF13)、EVENT OUT (AF15) | -（扩展引脚）               |
| PC7      | I/O  | PC7          | I2S3_MCK (AF6)、USART6_RX (AF7)、TIM3_CH2 (AF2)、SDIO_D7 (AF13)、EVENT OUT (AF15) | -（扩展引脚）               |
| PC8      | I/O  | PC8          | USART6_CK (AF7)、TIM3_CH3 (AF2)、SDIO_D0 (AF13)、EVENT OUT (AF15) | -（扩展引脚）               |
| PC9      | I/O  | PC9          | I2S_CKIN (AF10)、I2C3_SDA (AF4)、TIM3_CH4 (AF2)、SDIO_D1 (AF13)、MCO2 (AF0)、EVENT OUT (AF15) | -（扩展引脚）               |
| PC10     | I/O  | PC10         |   SPI3_SCK/I2S3_CK (AF6)、SDIO_D2 (AF13)、EVENT OUT (AF15)   | -（扩展引脚）               |
| PC11     | I/O  | PC11         | I2S3EXT_SD (AF6)、SPI3_MISO (AF6)、SDIO_D3 (AF13)、EVENT OUT (AF15) | -（扩展引脚）               |
| PC12     | I/O  | PC12         |  SPI3_MOSI/I2S3_SD (AF6)、SDIO_CK (AF13)、EVENT OUT (AF15)   | -（扩展引脚）               |
| PC13     | I/O  | PC13         |                       EVENT OUT (AF15)                       | -（RTC 功能）               |
| PC14     | I/O  | PC14         |                       EVENT OUT (AF15)                       | -（OSC32_IN）               |
| PC15     | I/O  | PC15         |                       EVENT OUT (AF15)                       | -（OSC32_OUT）              |
| PD0      | I/O  | PD0          |                       EVENT OUT (AF15)                       | -（扩展引脚）               |
| PD1      | I/O  | PD1          |                       EVENT OUT (AF15)                       | -（扩展引脚）               |
| PD2      | I/O  | PD2          |      TIM3_ETR (AF2)、SDIO_CMD (AF13)、EVENT OUT (AF15)       | -（扩展引脚）               |
| PD3      | I/O  | PD3          | SPI2_SCK/I2S2_CK (AF10)、USART2_CTS (AF7)、EVENT OUT (AF15)  | -（扩展引脚）               |
| PD4      | I/O  | PD4          |              USART2_RTS (AF7)、EVENT OUT (AF15)              | -（扩展引脚）               |
| PD5      | I/O  | PD5          |              USART2_TX (AF7)、EVENT OUT (AF15)               | -（扩展引脚）               |
| PD6      | I/O  | PD6          |  SPI3_MOSI/I2S3_SD (AF6)、USART2_RX (AF7)、EVENT OUT (AF15)  | -（扩展引脚）               |
| PD7      | I/O  | PD7          |              USART2_CK (AF7)、EVENT OUT (AF15)               | -（扩展引脚）               |
| VDD      | 电源 | 主电源输入   |                              -                               | -                           |
| VSS      | 地   | 电源地       |                              -                               | -                           |
| VDDA     | 电源 | 模拟电源输入 |                              -                               | -                           |
| VSSA     | 地   | 模拟地       |                              -                               | -                           |
| NRST     | 输入 | 复位输入     |                       EVENT OUT (AF15)                       | -（复位按钮）               |
| OSC_IN   | 输入 | 高速晶振输入 |                       EVENT OUT (AF15)                       | -                           |
| OSC_OUT  | 输出 | 高速晶振输出 |                       EVENT OUT (AF15)                       | -                           |
| BOOT0    | 输入 | 启动模式选择 |                              -                               | -（启动配置引脚             |