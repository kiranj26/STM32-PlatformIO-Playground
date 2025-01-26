#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdbool.h>

/* ------------------------------------------------
   Configuration
   ------------------------------------------------ */
#define RXBUF_SIZE   128  // Ring buffer size
#define CMDLINE_SIZE  64  // Max single command length

/*
 * Global UART handle and ring buffer variables
 */
UART_HandleTypeDef huart1;

/* Ring buffer for RX data */
static volatile uint8_t rxRingBuf[RXBUF_SIZE];
static volatile uint16_t head = 0;  // Next free position
static volatile uint16_t tail = 0;  // Oldest available byte

/* We'll receive incoming bytes one at a time via interrupt. */
static uint8_t rxByte;

/* Command line buffer + index */
static char cmdLine[CMDLINE_SIZE];
static uint16_t cmdIndex = 0;

/*
 * Function Prototypes
 */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* Blocking transmit of a string */
static void print(const char *str);

/* Process a completed command line */
static void processCommand(const char *cmd);

/*
 * ------------------------------------------------
 * main()
 * ------------------------------------------------
 */
int main(void)
{
    /* 1) HAL init */
    HAL_Init();

    /* 2) Configure system clock (8 MHz HSI, no PLL) */
    SystemClock_Config();

    /* 3) Initialize GPIO (for LED on PA5) and USART1 */
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    /* 4) Start 1-byte interrupt-based RX. */
    HAL_UART_Receive_IT(&huart1, &rxByte, 1);

    print("\r\nRing Buffer UART Example\r\n");
    print("Type commands: help, led on, led off, ping, version\r\n");

    while (1)
    {
        /* Check if the ring buffer has data */
        while (tail != head)
        {
            /* Get next char from ring buffer */
            uint8_t c = rxRingBuf[tail];
            tail = (tail + 1) % RXBUF_SIZE;

            /* Check if it's newline => process command */
            if (c == '\r' || c == '\n')
            {
                /* Terminate cmdLine */
                cmdLine[cmdIndex] = '\0';

                if (cmdIndex > 0)
                {
                    /* Process the line */
                    processCommand(cmdLine);
                    cmdIndex = 0; // reset
                }
            }
            else
            {
                /* Accumulate into cmdLine if space remains */
                if (cmdIndex < (CMDLINE_SIZE - 1))
                {
                    cmdLine[cmdIndex++] = (char)c;
                }
                else
                {
                    print("Command too long, reset.\r\n");
                    cmdIndex = 0;
                }
            }
        }
        // ... Possibly do other tasks
    }
}

/*
 * ------------------------------------------------
 * HAL_UART_RxCpltCallback
 * ------------------------------------------------
 * Called whenever 1 byte is received via interrupt.
 * We store that byte in the ring buffer, then re-arm.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // Store byte in ring buffer
        rxRingBuf[head] = rxByte;
        head = (head + 1) % RXBUF_SIZE;

        // Optional: handle ring buffer overflow if head == tail

        // Re-arm to receive next byte
        HAL_UART_Receive_IT(&huart1, &rxByte, 1);
    }
}

/*
 * ------------------------------------------------
 * processCommand()
 * ------------------------------------------------
 * Simple command parser: help, led on/off, ping,
 * version. Unknown => "Unknown command".
 */
static void processCommand(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        print("Commands:\r\n  help\r\n  led on\r\n  led off\r\n  ping\r\n  version\r\n");
    }
    else if (strcmp(cmd, "led on") == 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        print("LED ON\r\n");
    }
    else if (strcmp(cmd, "led off") == 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        print("LED OFF\r\n");
    }
    else if (strcmp(cmd, "ping") == 0)
    {
        print("pong\r\n");
    }
    else if (strcmp(cmd, "version") == 0)
    {
        print("v1.0.0\r\n");
    }
    else
    {
        print("Unknown command\r\n");
    }
}

/*
 * ------------------------------------------------
 * print()
 * ------------------------------------------------
 * Blocking transmit of a string for simplicity
 */
static void print(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/*
 * ------------------------------------------------
 * SystemClock_Config()
 * ------------------------------------------------
 * Use internal HSI @ 8 MHz, no PLL.
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    // 1) HSI on
    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1);
    }

    // 2) SysClk = HSI
    RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
                                        RCC_CLOCKTYPE_HCLK   |
                                        RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while (1);
    }
}

/*
 * ------------------------------------------------
 * MX_GPIO_Init()
 * ------------------------------------------------
 * Enable GPIOA, config PA5 as LED output
 */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/*
 * ------------------------------------------------
 * MX_USART1_UART_Init()
 * ------------------------------------------------
 * Configure PA9 (TX), PA10 (RX) @ 115200, 8N1.
 * Also enable NVIC for USART1.
 */
static void MX_USART1_UART_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // TX/RX pins in AF1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // UART config
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

    // Enable USART1 interrupts in NVIC
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/*
 * ------------------------------------------------
 * USART1_IRQHandler()
 * ------------------------------------------------
 * If not in a separate stm32f0xx_it.c, define it here:
 */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* 
 * (Optional) SysTick_Handler if needed:
 *
void SysTick_Handler(void)
{
    HAL_IncTick();
}
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
}