# STM32 Nucleo-F030R8: Blink LED & USART1 (PA9/PA10) Echo Example

This repository contains a minimal example project for the **STM32 Nucleo-F030R8** development board. It demonstrates:

1. **Blinking the onboard User LED** on `PA5`.
2. **Initializing USART1** on `PA9` (TX) and `PA10` (RX) at `115200 baud`.
3. **Printing** `"UART is initialised\r\n"` at startup.
4. **Echoing** any characters received on USART1 back to the sender.

The project is configured to use **PlatformIO** with the **STM32Cube HAL** framework.

---

## Table of Contents

1. [Features](#features)
2. [Hardware Requirements](#hardware-requirements)
3. [Project Structure](#project-structure)
4. [Quick Start](#quick-start)
5. [Usage](#usage)
6. [UART Terminal Screenshot](#uart-terminal-screenshot)
7. [Code Overview](#code-overview)
8. [Troubleshooting](#troubleshooting)
9. [License](#license)
10. [Contributing](#contributing)

---

## Features

- **HAL-Driven**: Uses the official STM32Cube HAL drivers for initialization and peripheral control.  
- **Blink**: Toggles the onboard LED (`PA5`) every 500 ms.  
- **USART1**: Configured for `PA9 (TX)` and `PA10 (RX)` at `115200 8N1`.  
- **Echo**: A simple echo loop that sends back any received byte on USART1.  
- **SysTick**: Properly handles SysTick so that `HAL_Delay()` functions as intended.

---

## Hardware Requirements

- **Nucleo-F030R8** board  
- **USB-to-Serial Adapter** (FTDI, CP2102, CH340, etc.)  
  - Connect the adapter’s `RX` → `PA9` (Nucleo TX)  
  - Connect the adapter’s `TX` → `PA10` (Nucleo RX)  
  - Common **GND** shared between the adapter and the Nucleo

> **Note**: The onboard ST-Link Virtual COM port on the Nucleo-F030R8 is typically connected to **USART2** (`PA2`/`PA3`), **not** USART1. Therefore, you **must** use an external USB-to-Serial adapter on `PA9/PA10` to see this example’s output.

---

## Project Structure

```
.
├── include/           # Optional headers
├── lib/               # Optional libraries
├── src/
│   └── main.c         # Main application code
├── docs/
│   └── images/
│       └── uart_terminal_placeholder.png  # Placeholder for your UART screenshot
└── platformio.ini     # PlatformIO configuration
```

- **`main.c`** contains all the initialization and the main `while (1)` loop with blink and echo logic.
- **`platformio.ini`** configures the build environment for Nucleo-F030R8 under STM32Cube.
- **`docs/images/uart_terminal_placeholder.png`** is a placeholder for your screenshot. Replace or rename it as needed.

---

## Quick Start

1. **Install PlatformIO**  
   - [Official Installation Guide](https://docs.platformio.org/en/latest/installation.html)

2. **Clone or Download** this repository:
   ```bash
   git clone https://github.com/your_username/stm32f030r8_blink_uart_example.git
   ```
   *(Replace the URL above with your actual repo URL.)*

3. **Open** the folder in your preferred IDE (e.g., VS Code with the PlatformIO extension).

4. **Connect** the Nucleo-F030R8 board via USB.  
   - If you have ST-Link drivers installed, PlatformIO will detect it for programming.

5. **Hook Up** your USB-to-Serial adapter to PA9/PA10 (see [Hardware Requirements](#hardware-requirements)).

6. **Build & Upload**:
   ```bash
   pio run
   pio run --target upload
   ```
   Or use the PlatformIO UI in VS Code.

7. **Open a Terminal** (PuTTY, screen, minicom, etc.) on the USB-to-Serial adapter’s COM port:
   - Baud: `115200`
   - Data Bits: `8`
   - Parity: `None`
   - Stop Bits: `1`
   - Flow Control: `None`

8. **Observe**:
   - An LED blinking on `PA5` every 500 ms.
   - One-time print: `"UART is initialised\r\n"`.
   - Echo of any characters you type in the terminal.

---

## Usage

- After flashing, reset or power-cycle the Nucleo board.
- The onboard LED (at `PA5`) blinks regularly, confirming the main loop is running.
- A single message `"UART is initialised\r\n"` appears in your terminal.
- Any character you type in the terminal is echoed back.

### Changing Baud Rate
- If you prefer a different baud rate, edit `huart1.Init.BaudRate` in `MX_USART1_UART_Init()` (within `main.c`).

### Changing LED Pin
- The user LED on Nucleo-F030R8 is typically `PA5`. If your board uses a different pin, update `MX_GPIO_Init()` accordingly.

---

## UART Terminal Screenshot

Here is a placeholder image showing what your terminal **might** look like after startup and during echo testing:

![UART Terminal Placeholder](docs/images/uart_terminal_placeholder.png "UART Terminal Placeholder")

*(Replace this placeholder with your actual screenshot to document how it looks in a real setup.)*

---

## Code Overview

### `SystemClock_Config()`
- Configures the system to run at **8 MHz** from the internal HSI.  
- If you want **48 MHz** (HSI * 6 PLL), modify the PLL settings in this function.

### `MX_GPIO_Init()`
- Enables **GPIOA** clock.  
- Sets **PA5** as output (push-pull, no pull-up/down) for the user LED.

### `MX_USART1_UART_Init()`
- Enables **USART1** and **GPIOA** clocks.  
- Configures **PA9** as TX and **PA10** as RX (alternate function AF1).  
- Initializes UART to **115200, 8N1, no parity**.

### `main()` Loop
1. **Blink**: Toggles `PA5` every 500 ms (`HAL_Delay(500)`).  
2. **UART Echo**: Checks for received byte (`HAL_UART_Receive(...)`) and, if available, transmits it back.

### SysTick Handler
- The **STM32Cube HAL** uses `SysTick` to generate a 1ms time base.  
- Make sure the default `SysTick_Handler()` calls `HAL_IncTick();` so `HAL_Delay()` works.

---

## Troubleshooting

1. **No Output / LED Not Blinking**  
   - Check that the code actually flashed onto the board.  
   - Confirm `SystemClock_Config()` isn’t failing.  
   - Ensure `SysTick_Handler()` calls `HAL_IncTick()`.

2. **Garbled / Partial Serial Output**  
   - Verify baud rate (115200) and correct COM port in your terminal.  
   - Confirm you’re physically connected to **PA9/PA10** with an external USB-to-Serial adapter.

3. **No Echo**  
   - Double-check wiring: **PA9 → RX**, **PA10 → TX**, shared GND.  
   - Ensure you haven’t accidentally opened the ST-Link Virtual COM port (which is typically USART2).

4. **Use a Debugger**  
   - In `platformio.ini`, specify `upload_protocol = stlink` and `debug_tool = stlink`.  
   - Run `pio debug` to set breakpoints and inspect variables.

---

## License

This example is provided under the [MIT License](LICENSE) (or whichever license you prefer). You are free to use, modify, and distribute it in your own projects. See the `LICENSE` file for details.

---

## Contributing

Feel free to open issues or pull requests if you find improvements or bugs. This is a bare-bones example intended to help you get started with **STM32F030** development on the **Nucleo-F030R8** board.

---

**Happy Coding!**  
If you have any questions or feedback, please open an issue or reach out to the maintainers.
