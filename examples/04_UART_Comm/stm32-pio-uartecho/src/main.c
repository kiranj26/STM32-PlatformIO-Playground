#include "stm32f0xx_hal.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/**
  * @brief  The application entry point.
  */
int main(void)
{
    /* 1. Initialize the HAL library (includes setting up SysTick) */
    HAL_Init();

    /* 2. Configure the system clock (8 MHz from HSI, no PLL here) */
    SystemClock_Config();

    /* 3. Initialize GPIO (for LED on PA5) and USART1 (PA9/PA10) */
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    /* 4. Print "UART is initialised" message */
    const char *initMsg = "UART is initialised\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)initMsg, strlen(initMsg), HAL_MAX_DELAY);

    /* 5. Main loop: blink LED and do echo */
    while (1)
    {
        /* A) Toggle the user LED on PA5 every 500ms */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);

        /* B) Echo any received bytes (non-blocking check) */
        uint8_t rxByte;
        if (HAL_UART_Receive(&huart1, &rxByte, 1, 1) == HAL_OK)
        {
            HAL_UART_Transmit(&huart1, &rxByte, 1, HAL_MAX_DELAY);
        }
    }
}

/**
  * @brief  System Clock Configuration
  *         HSI = 8 MHz, no PLL, SysClk = 8 MHz.
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable PWR clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 1) Configure HSI oscillator */
    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE; // No PLL
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* 2) Initialize CPU, AHB, and APB clocks to 8MHz */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;  // 8MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

/**
  * @brief GPIO Initialization Function
  *        Configures PA5 as push-pull output for the onboard user LED.
  */
static void MX_GPIO_Init(void)
{
    /* Enable GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure PA5 for the user LED */
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Optional: start with LED off */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/**
  * @brief Initialize USART1 on PA9 (TX) and PA10 (RX) at 115200 baud.
  */
static void MX_USART1_UART_Init(void)
{
    /* Enable USART1 and GPIOA clocks */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure PA9 (TX) and PA10 (RX) in Alternate Function (AF1) */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure the UART parameters */
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

/* --------------------------------------------------------------------------
   IMPORTANT: SysTick Handler for the HAL time base
   --------------------------------------------------------------------------
   By default, STM32Cube projects include a weak SysTick_Handler() that calls 
   HAL_IncTick(). If yours doesn't, or if you need to override it, do:
*/

void SysTick_Handler(void)
{
    HAL_IncTick();
}

