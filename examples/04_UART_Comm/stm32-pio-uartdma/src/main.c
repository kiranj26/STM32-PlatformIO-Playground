#include "stm32f0xx_hal.h"
#include <string.h>   // for strlen, memcpy
#include <stdbool.h>  // for bool

/* -------------------------------------------------------------------------
   Global Handles & Buffers
   ------------------------------------------------------------------------- */
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* We'll receive exactly 4 bytes, then echo them. */
#define RX_SIZE 4

/* The initial greeting message to send at startup */
static uint8_t txGreeting[] = "DMA UART Demo: Send 4 bytes to see LED toggle + echo!\r\n";

/* Buffer to hold the 4 incoming bytes */
static uint8_t rxBuffer[RX_SIZE];

/* We’ll build an echoed message by appending \r\n to the 4 bytes */
static uint8_t txEchoBuf[RX_SIZE + 2];

/* Flag to indicate if a DMA TX is currently in progress */
volatile bool txInProgress = false;

/* -------------------------------------------------------------------------
   Function Prototypes
   ------------------------------------------------------------------------- */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);

/* -------------------------------------------------------------------------
   main()
   ------------------------------------------------------------------------- */
int main(void)
{
    /* 1. Init HAL (sets up SysTick, etc.) */
    HAL_Init();

    /* 2. Configure system clock (8 MHz HSI, no PLL) */
    SystemClock_Config();

    /* 3. Initialize peripherals: GPIO (LED), DMA, USART1 */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();

    /* 4. Transmit a greeting message once via DMA */
    txInProgress = true;
    if (HAL_UART_Transmit_DMA(&huart1, txGreeting, strlen((char*)txGreeting)) != HAL_OK)
    {
        /* If TX start fails, stay here */
        while (1);
    }

    /* 5. Start a DMA-based reception of 4 bytes */
    if (HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_SIZE) != HAL_OK)
    {
        /* If RX start fails, stay here */
        while (1);
    }

    /* 6. Main loop does nothing else. 
          LED toggles & echo happen in the RX callback. */
    while (1)
    {
        // Idle. Everything is callback-driven.
    }
}

/* -------------------------------------------------------------------------
   SystemClock_Config
   Using HSI (8 MHz) as system clock, no PLL.
-------------------------------------------------------------------------- */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable power clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 1) HSI on, no PLL */
    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1); // Error
    }

    /* 2) Set SYSCLK source, no division */
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | 
                                        RCC_CLOCKTYPE_HCLK   | 
                                        RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while (1); // Error
    }
}

/* -------------------------------------------------------------------------
   GPIO Init
   - Enable GPIOA clock
   - Configure PA5 (User LED) as push-pull output
-------------------------------------------------------------------------- */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Start LED off */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* -------------------------------------------------------------------------
   DMA Init
   - Enable DMA1 clock
   - Configure NVIC for DMA1 Channel2_3 (used by USART1 TX/RX)
-------------------------------------------------------------------------- */
static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* NVIC config for DMA1 Channel 2 & 3 */
    HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

/* -------------------------------------------------------------------------
   USART1 Init
   - PA9 => TX (AF1), PA10 => RX (AF1)
   - 115200 8N1
   - Link DMA channels:
       USART1_TX => DMA1_Channel2
       USART1_RX => DMA1_Channel3
-------------------------------------------------------------------------- */
static void MX_USART1_UART_Init(void)
{
    /* Enable USART1 clock */
    __HAL_RCC_USART1_CLK_ENABLE();

    /* Configure PA9 and PA10 in Alternate Function mode for USART1 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Basic UART param config */
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
        while (1);
    }

    /* ----------------------
       DMA for TX (Channel 2)
       ---------------------- */
    hdma_usart1_tx.Instance                 = DMA1_Channel2;
    hdma_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode                = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority            = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
        while (1);
    }
    /* Link TX DMA to UART1 */
    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

    /* ----------------------
       DMA for RX (Channel 3)
       ---------------------- */
    hdma_usart1_rx.Instance                 = DMA1_Channel3;
    hdma_usart1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode                = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority            = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
        while (1);
    }
    /* Link RX DMA to UART1 */
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
}

/* -------------------------------------------------------------------------
   DMA IRQ Handler for Channels 2 & 3
-------------------------------------------------------------------------- */
void DMA1_Channel2_3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

/* -------------------------------------------------------------------------
   UART Callbacks
-------------------------------------------------------------------------- */

/* TX Complete Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        txInProgress = false;

        /* // Another debug message:
        char doneMsg[] = "DMA TX complete.\r\n";
        HAL_UART_Transmit(huart, (uint8_t*)doneMsg, strlen(doneMsg), HAL_MAX_DELAY); */

    }
}

/* RX Complete Callback (4 bytes arrived) */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Toggle LED so we know 4 bytes arrived */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        /* 
        // ---- Blocking debug print ---- 
        char debugMsg[] = "Got 4 bytes! Echoing...\r\n";
        // Blocking transmit to ensure it definitely appears if TX lines are correct
        HAL_UART_Transmit(huart, (uint8_t*)debugMsg, strlen(debugMsg), HAL_MAX_DELAY); */

        /* Build an echo with \r\n appended */
        memcpy(txEchoBuf, rxBuffer, RX_SIZE);
        txEchoBuf[RX_SIZE]   = '\r';
        txEchoBuf[RX_SIZE+1] = '\n';

        /* Send that echo if TX is free */
        if (!txInProgress)
        {
            txInProgress = true;
            HAL_UART_Transmit_DMA(&huart1, txEchoBuf, RX_SIZE + 2);
        }

        /* Restart RX for another 4 bytes */
        HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_SIZE);
    }
}

/* Optional SysTick_Handler if needed for HAL_Delay() usage
   (Here, we don't call HAL_Delay, so we might not need it.)
   Uncomment if needed:

void SysTick_Handler(void)
{
    HAL_IncTick();
}
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
}
