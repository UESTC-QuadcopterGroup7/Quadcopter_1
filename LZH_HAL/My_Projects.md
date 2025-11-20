# HAL库环境配置
本项目采用官方的2.9.0的pack，在根目录下一般位于`C:\Keil_v5\ARM\PACK\Keil\STM32F4xx_DFP\2.9.0`

- 配置环境时可以使用CMD运行F4CreateProj.bat脚本，将自动从pack里面复制相关文件到项目目录下
## 使用方法
```bash
F4CreateProj.bat [项目名称]
```
- 确保脚本和keil_proj目录在同一级目录下
## 项目结构

创建的项目将包含以下目录结构：

```
项目名称/
├── 项目名称.uvprojx          # Keil项目文件
├── Src/                      # 源文件目录
│   ├── main.c               # 主程序文件（LED闪烁示例）
│   └── stm32f4xx_it.c       # 中断处理文件
├── Inc/                      # 头文件目录
│   ├── main.h               # 主程序头文件
│   ├── stm32f4xx_hal_conf.h # HAL库配置文件
│   └── stm32f4xx_it.h       # 中断处理头文件
└── Drivers/                  # 驱动库目录
├── STM32F4xx_HAL_Driver/ # HAL驱动库
│   ├── Inc/             # HAL头文件
│   └── Src/             # HAL源文件
└── CMSIS/               # CMSIS文件
├── Include/         # CMSIS头文件
└── Device/          # 设备支持文件
```