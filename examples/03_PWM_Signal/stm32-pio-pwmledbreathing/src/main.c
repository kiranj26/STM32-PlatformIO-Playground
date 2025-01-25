/*
 * File: main.c
 * Author: Kiran Jojare
 * Project: STM32 PlatformIO Playground - Breathing LED Example
 * Description:
 * This code creates a smooth "breathing" LED effect on the onboard LED (PA5).
 * The PWM duty cycle varies sinusoidally to simulate the breathing effect.
 *
 * Hardware Setup:
 * - Configure PA5 (onboard LED) in analog mode (high impedance).
 * - Generate PWM on PA6 using Timer 3, Channel 1.
 * - Connect PA6 to PA5 with a jumper wire.
 */

#include "stm32f0xx_hal.h"
#include <math.h> // For sine wave calculation

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);

TIM_HandleTypeDef htim3;

int main(void) {
    // Initialize the HAL Library
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize GPIO and Timer
    MX_GPIO_Init();
    MX_TIM3_Init();

    // Start PWM on Timer 3, Channel 1
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    uint32_t dutyCycle;
    float phase = 0.0f; // Phase for sine wave

    // Main loop
    while (1) {
        // Calculate duty cycle as a sine wave
        dutyCycle = (uint32_t)((sinf(phase) + 1) * 500); // Scale sine value to 0-1000

        // Update PWM duty cycle
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, dutyCycle);

        // Increment phase and wrap around
        phase += 0.05f; // Controls breathing speed
        if (phase >= 2 * M_PI) {
            phase -= 2 * M_PI; // Keep phase within 0 to 2*PI
        }

        HAL_Delay(10); // Smooth transition
    }
}

// SysTick interrupt handler (required for HAL_Delay)
void SysTick_Handler(void) {
    HAL_IncTick();
}

// System clock configuration
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

// GPIO initialization
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIOA clock
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure PA5 in Analog mode (high impedance)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure PA6 for PWM (Timer 3 Channel 1)
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Timer 3 initialization for PWM
static void MX_TIM3_Init(void) {
    __HAL_RCC_TIM3_CLK_ENABLE(); // Enable Timer 3 clock

    TIM_OC_InitTypeDef sConfigOC = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 48 - 1;  // Prescaler for 1 MHz timer clock
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000 - 1;  // Period for 1 kHz PWM signal
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    // Configure PWM Channel 1
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;  // Initial duty cycle (0%)
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
}
