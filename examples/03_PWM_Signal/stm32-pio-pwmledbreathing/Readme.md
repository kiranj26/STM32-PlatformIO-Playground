
# Breathing LED Effect on STM32 Nucleo F030R8

## Overview

This example demonstrates how to create a **breathing LED effect** on the onboard LED (connected to PA5) of the STM32 Nucleo F030R8. The effect simulates a smooth, rhythmic transition in brightness, akin to the natural rhythm of breathing. A **PWM signal** is used to control the LED brightness with a sinusoidal variation in duty cycle.

---
## Video Explanation

*Placeholder for adding a video link or description of how the breathing effect works.*

- **[Add your video description or link here]**

---

## Why This Approach?

### Challenge:
The onboard LED is hardwired to GPIO **PA5**, which cannot directly generate a PWM signal using a timer.

### Solution:
1. Configure **PA5** in **analog mode** (high impedance) to allow external control.
2. Use **PA6**, which supports PWM output using **Timer 3 Channel 1**.
3. Physically connect **PA6** to **PA5** using a jumper wire.
4. Vary the PWM duty cycle on PA6 sinusoidally to create the breathing effect.

---

## How It Works

1. **Timer 3 Configuration**:
   - Timer 3 generates a PWM signal on **PA6**.
   - The PWM signal has a frequency of **1 kHz**.

2. **Sinusoidal Duty Cycle**:
   - The duty cycle is calculated as:
     ```c
     dutyCycle = (uint32_t)((sinf(phase) + 1) * 500);
     ```
   - This creates a smooth variation in brightness.

3. **Hardware Setup**:
   - Configure **PA5** in analog mode (high impedance).
   - Connect **PA6** (PWM output) to **PA5** using a jumper wire.

4. **Breathing Effect**:
   - The LED brightness smoothly transitions from OFF to FULLY ON and back in a sinusoidal pattern.

---

## Observations

- The onboard LED:
  - Gradually **brightens** (fades in) as the duty cycle increases.
  - Gradually **dims** (fades out) as the duty cycle decreases.
- The breathing speed can be adjusted by modifying the phase increment.

---

## Hardware Requirements

1. **STM32 Nucleo F030R8** board.
2. Jumper wire to connect **PA6** to **PA5**.

---

## How to Run

1. **Clone the repository** and navigate to the `03_Breathing_LED` example directory.
   ```bash
   git clone https://github.com/yourusername/STM32-PlatformIO-Playground.git
   cd STM32-PlatformIO-Playground/examples/03_Breathing_LED
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

## Customization

1. **Adjust Breathing Speed**:
   - Modify the `phase` increment in the code:
     ```c
     phase += 0.05f; // Increase for faster breathing, decrease for slower breathing
     ```

2. **Experiment with Frequencies**:
   - Change the PWM frequency by adjusting the timer configuration.

---

## Future Improvements

1. Apply the breathing effect to multiple LEDs using multiple PWM channels.
2. Implement dynamic control of the breathing speed via a button or external input.
3. Add FreeRTOS tasks for more complex lighting effects.

---

## Acknowledgements

This project is part of the **STM32 PlatformIO Playground** repository, showcasing examples for learning STM32 peripherals.

---
