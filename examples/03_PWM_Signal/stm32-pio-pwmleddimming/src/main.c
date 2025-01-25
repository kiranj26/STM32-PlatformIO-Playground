/*
 * File: main.c
 * Author: Kiran Jojare
 * Project: STM32 PlatformIO Playground - PWM Example
 * Description:
 * This code demonstrates how to use PWM (Pulse Width Modulation) to control the brightness of an LED.
 * The onboard LED (connected to PA5) is configured to fade in and out smoothly using a PWM signal
 * generated on another GPIO pin (PA6).
 *
 * Why Use an External GPIO for PWM with the Onboard LED:
 * - The onboard LED is hardwired to GPIO PA5, which cannot directly be configured as a timer-controlled PWM output.
 * - To achieve PWM control of the onboard LED, we:
 *   1. Configure PA5 in "Analog Mode", setting it to high impedance.
 *   2. Use PA6, which is capable of PWM output via TIM3 Channel 1.
 *   3. Connect PA6 (PWM output) to PA5 (onboard LED) using a jumper wire.
 *   4. Vary the PWM duty cycle to simulate brightness changes on the onboard LED.
 *
 * What This Code Does:
 * - Initializes the STM32 HAL library and configures the system clock.
 * - Configures PA5 in "Analog Mode" to make it high impedance.
 * - Configures PA6 as a PWM output using Timer 3 Channel 1.
 * - Outputs a PWM signal on PA6, gradually varying the duty cycle between 0% and 100%.
 * - The onboard LED connected to PA5 fades in and out due to the PWM signal applied indirectly via PA6.
 *
 * Hardware Setup:
 * - Connect GPIO PA6 to PA5 using a jumper wire.
 *
 * Observations:
 * - The onboard LED (PA5) will gradually fade from OFF to fully ON and back to OFF in a loop.
 *
 * Note:
 * - The PWM signal generated on PA6 is within the permissible voltage range for PA5.
 */

#include "stm32f0xx_hal.h"

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

    uint32_t dutyCycle = 0;
    uint8_t increasing = 1;

    // Main loop
    while (1) {
        // Adjust the duty cycle
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, dutyCycle);

        // Increment or decrement duty cycle
        if (increasing) {
            dutyCycle += 10;  // Increase brightness
            if (dutyCycle >= 1000) increasing = 0;  // Reverse direction
        } else {
            dutyCycle -= 10;  // Decrease brightness
            if (dutyCycle == 0) increasing = 1;  // Reverse direction
        }

        HAL_Delay(10); // Smooth fading
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
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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

    // Configure PA5 in Analog mode (disables GPIO control, sets high impedance)
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
