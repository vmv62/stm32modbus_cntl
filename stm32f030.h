#define FLASH_BASE            ((uint32_t)0x08000000) /*!< FLASH base address in the alias region */
#define SRAM_BASE             ((uint32_t)0x20000000) /*!< SRAM base address in the alias region */
#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */

/*!< Peripheral memory map */
#define APBPERIPH_BASE        PERIPH_BASE
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x08000000)

#define TIM2_BASE             (APBPERIPH_BASE + 0x00000000)
#define TIM3_BASE             (APBPERIPH_BASE + 0x00000400)
#define TIM6_BASE             (APBPERIPH_BASE + 0x00001000)
#define TIM14_BASE            (APBPERIPH_BASE + 0x00002000)
#define RTC_BASE              (APBPERIPH_BASE + 0x00002800)
#define WWDG_BASE             (APBPERIPH_BASE + 0x00002C00)
#define IWDG_BASE             (APBPERIPH_BASE + 0x00003000)
#define SPI2_BASE             (APBPERIPH_BASE + 0x00003800)
#define USART2_BASE           (APBPERIPH_BASE + 0x00004400)
#define I2C1_BASE             (APBPERIPH_BASE + 0x00005400)
#define I2C2_BASE             (APBPERIPH_BASE + 0x00005800)
#define PWR_BASE              (APBPERIPH_BASE + 0x00007000)
#define DAC_BASE              (APBPERIPH_BASE + 0x00007400)
#define CEC_BASE              (APBPERIPH_BASE + 0x00007800)

#define SYSCFG_BASE           (APBPERIPH_BASE + 0x00010000)
#define COMP_BASE             (APBPERIPH_BASE + 0x0001001C)
#define EXTI_BASE             (APBPERIPH_BASE + 0x00010400)
#define ADC1_BASE             (APBPERIPH_BASE + 0x00012400)
#define ADC_BASE              (APBPERIPH_BASE + 0x00012708)
#define TIM1_BASE             (APBPERIPH_BASE + 0x00012C00)
#define SPI1_BASE             (APBPERIPH_BASE + 0x00013000)
#define USART1_BASE           (APBPERIPH_BASE + 0x00013800)
#define TIM15_BASE            (APBPERIPH_BASE + 0x00014000)
#define TIM16_BASE            (APBPERIPH_BASE + 0x00014400)
#define TIM17_BASE            (APBPERIPH_BASE + 0x00014800)
#define DBGMCU_BASE           (APBPERIPH_BASE + 0x00015800)

#define DMA1_BASE             (AHBPERIPH_BASE + 0x00000000)
#define DMA1_Channel1_BASE    (DMA1_BASE + 0x00000008)
#define DMA1_Channel2_BASE    (DMA1_BASE + 0x0000001C)
#define DMA1_Channel3_BASE    (DMA1_BASE + 0x00000030)
#define DMA1_Channel4_BASE    (DMA1_BASE + 0x00000044)
#define DMA1_Channel5_BASE    (DMA1_BASE + 0x00000058)
#define RCC_BASE              (AHBPERIPH_BASE + 0x00001000)
#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x00002000) /*!< FLASH registers base address */
#define OB_BASE               ((uint32_t)0x1FFFF800)        /*!< FLASH Option Bytes base address */
#define CRC_BASE              (AHBPERIPH_BASE + 0x00003000)
#define TSC_BASE              (AHBPERIPH_BASE + 0x00004000)

#define GPIOA_BASE            (AHB2PERIPH_BASE + 0x00000000)
#define GPIOB_BASE            (AHB2PERIPH_BASE + 0x00000400)
#define GPIOC_BASE            (AHB2PERIPH_BASE + 0x00000800)
#define GPIOD_BASE            (AHB2PERIPH_BASE + 0x00000C00)
#define GPIOF_BASE            (AHB2PERIPH_BASE + 0x00001400)
