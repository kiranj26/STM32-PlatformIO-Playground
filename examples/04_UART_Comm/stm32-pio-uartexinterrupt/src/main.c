#include "stm32f0xx_hal.h"
#include <string.h>

/* --------------------------------------------------------------------------
   Global variables
   -------------------------------------------------------------------------- */
UART_HandleTypeDef huart1;

/* We'll store one incoming byte in this buffer for each interrupt */
volatile uint8_t rxByte = 0;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* --------------------------------------------------------------------------
   main()
   -------------------------------------------------------------------------- */
int main(void)
{
    /* 1. Initialize HAL (including SysTick for HAL_Delay) */
    HAL_Init();

    /* 2. Configure system clock (8 MHz HSI, no PLL) */
    SystemClock_Config();

    /* 3. Initialize GPIO (LED on PA5) and USART1 */
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    /* 4. Print initial message */
    const char *initMsg = "UART Interrupt Example: Initialized\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)initMsg, strlen(initMsg), HAL_MAX_DELAY);

    /* 5. Start the first receive interrupt for 1 byte */
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxByte, 1);

    /* 6. Main loop */
    while (1)
    {
        /* Blink LED every 500 ms so we know MCU is running */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);
    }
}

/* --------------------------------------------------------------------------
   System Clock Configuration
   --------------------------------------------------------------------------
   HSI = 8 MHz, no PLL => SystemCoreClock=8 MHz
   Adjust if you want 48 MHz by enabling PLL, etc.
-------------------------------------------------------------------------- */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable power interface clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 1) Initialize the RCC Oscillators (HSI) */
    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* 2) Initialize the CPU, AHB, and APB busses clocks */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI; // 8 MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

/* --------------------------------------------------------------------------
   GPIO Initialization
   --------------------------------------------------------------------------
   - Enable clock for GPIOA
   - Configure PA5 as output (User LED on many Nucleo boards)
-------------------------------------------------------------------------- */
static void MX_GPIO_Init(void)
{
    /* Enable GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure PA5 for the user LED (Push-Pull, no Pull-up) */
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Start with LED off */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* --------------------------------------------------------------------------
   USART1 Initialization
   --------------------------------------------------------------------------
   - PA9 => TX, PA10 => RX (AF1)
   - 115200, 8N1, no flow control
   - Enable interrupt in NVIC
-------------------------------------------------------------------------- */
static void MX_USART1_UART_Init(void)
{
    /* 1) Enable clocks */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 2) Configure GPIO pins for TX & RX in AF mode */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 3) Configure UART parameters */
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
        while (1); // Error
    }

    /* 4) Enable USART1 interrupts in NVIC (if not done automatically) */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* --------------------------------------------------------------------------
   UART Receive Complete Callback
   --------------------------------------------------------------------------
   This function is called by HAL when a byte is received on huart1.
-------------------------------------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Echo the received byte back (non-blocking) */
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)&rxByte, 1);

        /* Prepare to receive next byte */
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxByte, 1);
    }
}

/* --------------------------------------------------------------------------
   UART Transmit Complete Callback
   --------------------------------------------------------------------------
   You can add code here if you need to do something after a transmit finishes.
-------------------------------------------------------------------------- */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* For this example, we don't need extra actions after TX completes. */
}

/* --------------------------------------------------------------------------
   Interrupt Handler for USART1
   --------------------------------------------------------------------------
   If you're using Cube-generated code, you might have an stm32f0xx_it.c that
   defines this. If not, you can define it here:
-------------------------------------------------------------------------- */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* --------------------------------------------------------------------------
   SysTick_Handler
   --------------------------------------------------------------------------
   Ensure SysTick calls HAL_IncTick so HAL_Delay works
-------------------------------------------------------------------------- */
/* If not defined elsewhere, un-comment this:
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
}

