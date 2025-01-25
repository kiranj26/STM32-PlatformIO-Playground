
# PWM Fading Example on STM32 Nucleo F030R8

## Overview

This example demonstrates how to use **PWM (Pulse Width Modulation)** to control the brightness of the onboard LED (connected to PA5) on the STM32 Nucleo F030R8 board. The LED brightness smoothly fades in and out by varying the PWM duty cycle over time.

---

## Why This Approach?

### Challenge:
The onboard LED is hardwired to GPIO **PA5**, which cannot directly generate a PWM signal using a timer.

### Solution:
1. Configure **PA5** in **analog mode** (high impedance) to allow external control.
2. Use **PA6**, which supports PWM output using **Timer 3 Channel 1**.
3. Physically connect **PA6** to **PA5** using a jumper wire.
4. Vary the PWM duty cycle on PA6 to control the brightness of the onboard LED indirectly.

---

## How It Works

1. **Timer 3 Configuration**:
   - The timer generates a PWM signal on **PA6**.
   - The signal frequency is set to **1 kHz**.
   - The duty cycle is varied between **0%** (LED OFF) and **100%** (LED FULLY ON).

2. **GPIO Configuration**:
   - **PA5** is set to **analog mode** (high impedance).
   - **PA6** is configured as a **PWM output**.

3. **Fading Effect**:
   - The duty cycle is gradually increased and decreased in a loop.
   - The onboard LED brightness smoothly transitions from OFF to fully ON and back.

4. **Hardware Setup**:
   - Connect **PA6** to **PA5** with a jumper wire.

---

## Code Highlights

- **Duty Cycle Adjustment**:
  ```c
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, dutyCycle);
  ```

  - Updates the PWM duty cycle dynamically.

- **Fading Logic**:
  ```c
  if (increasing) {
      dutyCycle += 10;  // Increase brightness
      if (dutyCycle >= 1000) increasing = 0;
  } else {
      dutyCycle -= 10;  // Decrease brightness
      if (dutyCycle == 0) increasing = 1;
  }
  ```

  - Gradually changes the duty cycle between 0 and 1000 (0% to 100%).

---

## Observations

- As the PWM duty cycle changes, the onboard LED:
  - Gradually **brightens** (fades in) as the duty cycle increases.
  - Gradually **dims** (fades out) as the duty cycle decreases.

---

## Hardware Requirements

1. **STM32 Nucleo F030R8** board.
2. Jumper wire to connect **PA6** to **PA5**.

---

## Video Explanation

*Placeholder for adding a video link or description of how the fading works.*

- **[Add your video description or link here]**

---

## How to Run

1. **Clone the repository** and navigate to the `03_PWM_Signal` example directory.
   ```bash
   git clone https://github.com/yourusername/STM32-PlatformIO-Playground.git
   cd STM32-PlatformIO-Playground/examples/03_PWM_Signal
   ```

2. **Build the project** using PlatformIO:
   ```bash
   pio run
   ```

3. **Upload the firmware** to the STM32 Nucleo board:
   ```bash
   pio run --target upload
   ```

4. **Connect a jumper wire** between **PA6** and **PA5**.

5. Observe the onboard LED smoothly fading in and out.

---

## Future Improvements

1. Integrate additional GPIOs for controlling multiple LEDs with independent PWM signals.
2. Experiment with different PWM frequencies and observe the effect on LED brightness.
3. Use FreeRTOS to implement task-based brightness control.

---

## Acknowledgements

This project is part of the **STM32 PlatformIO Playground** repository, showcasing examples for learning STM32 peripherals.

---

