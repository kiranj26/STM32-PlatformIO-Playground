# STM32 PlatformIO Playground

Welcome to the **STM32 PlatformIO Playground**! This repository serves as a hands-on learning environment to explore various STM32 peripherals using PlatformIO as the sole IDE. The examples range from basic tasks like LED blinking to advanced peripheral operations such as UART communication, PWM, and more.

![Build Status - STM32f030r8](https://github.com/kiranj26/STM32-PlatformIO-Playground/actions/workflows/build.yml/badge.svg)
![Build Status -F0x Series Multi-Board](https://github.com/kiranj26/STM32-PlatformIO-Playground/actions/workflows/build_f0_multiboard.yml/badge.svg?branch=multi-board-f0)


## Repository Structure

```plaintext
STM32-PlatformIO-Playground/
├── README.md             - Overview and instructions for the repository.
├── LICENSE               - Licensing terms for the repository.
├── .gitignore            - Ignored files and folders for version control.
├── examples/             - Peripheral-specific example projects.
│   ├── 01_LED_Blink/     - LED blink example using GPIO.
│   ├── 02_Button_Press/  - Button press and LED control example.
│   ├── 03_PWM_Signal/    - PWM signal generation example.
│   ├── 04_UART_Comm/     - UART communication example.
│   └── ...               - Future examples to be added here.
```

## Getting Started

### Prerequisites
1. **Hardware:**
   - STM32 Nucleo F030R8 (or other STM32 boards).
   - Jumper wires, LEDs, buttons, etc., as required by the examples.

2. **Software:**
   - [PlatformIO Core](https://platformio.org/install).
   - A text editor like VS Code with the PlatformIO extension.

### Initial Setup
1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/STM32-PlatformIO-Playground.git
   cd STM32-PlatformIO-Playground
   ```

2. Navigate to an example folder:
   ```bash
   cd examples/01_LED_Blink
   ```

3. Open the project in PlatformIO:
   ```bash
   pio project init --board nucleo_f030r8
   ```

4. Build and upload the code:
   ```bash
   pio run --target upload
   ```

5. Monitor the serial output (if applicable):
   ```bash
   pio device monitor --baud 115200
   ```

## Examples Overview

### 1. LED Blink
Learn how to blink an LED using GPIO. This example demonstrates configuring pins as outputs and toggling them at a fixed interval.

### 2. Button Press
Control an LED using a button. This example introduces GPIO input handling and basic debouncing.

### 3. PWM Signal
Generate a PWM signal to control devices like servo motors or LEDs. You'll learn how to configure timer peripherals for PWM.

### 4. UART Communication
Set up serial communication to transmit and receive data between the STM32 board and your computer.

*More examples will be added for peripherals like I2C, SPI, ADC, and advanced applications.*

## Contribution
Feel free to contribute new examples, improve existing ones, or suggest enhancements. Follow these steps to contribute:
1. Fork this repository.
2. Create a branch for your feature or bug fix.
3. Commit your changes and submit a pull request.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements
Thanks to the open-source community for making tools like PlatformIO and STM32CubeMX available for free use.
