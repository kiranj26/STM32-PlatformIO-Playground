# STM32 Nucleo-F030R8: UART + DMA (Short-Burst Echo Example)

This project demonstrates **DMA-based UART** on an **STM32F030R8** Nucleo board using **USART1** (pins `PA9`/`PA10`). It transmits one greeting message at startup, then waits for **4 consecutive bytes** to be received via DMA. Once 4 bytes arrive:

1. **LED on `PA5` toggles** (confirming data was received).  
2. **Those 4 bytes** are echoed back **with a `\r\n`** via DMA.  

The main loop remains free (no blocking), so you can easily expand it for other tasks.

---

## Table of Contents

1. [Features](#features)  
2. [Hardware Setup](#hardware-setup)  
3. [Project Structure](#project-structure)  
4. [Quick Start](#quick-start)  
5. [Usage](#usage)  
6. [Troubleshooting](#troubleshooting)  
7. [License](#license)  
8. [Contributing](#contributing)

---

## Features

- **DMA-Based TX** on **Channel 2**:  
  - Sends a greeting at startup: `"DMA UART Demo: Send 4 bytes to see LED toggle + echo!\r\n"`.  
  - Echoes any received 4-byte block (plus `\r\n`) non-blocking.

- **DMA-Based RX** on **Channel 3**:  
  - Configured in **Normal Mode** to wait for exactly **4 bytes**.  
  - Triggers `HAL_UART_RxCpltCallback()` once the buffer is fully received.

- **LED Toggle** on **PA5**:  
  - Confirms each 4-byte RX event.  
  - Helps debug if you can’t see short data in your terminal.

- **Non-Blocking**:  
  - The CPU is mostly idle in `while(1)`.  
  - The DMA handles data movement, then calls callbacks when complete.

---

## Hardware Setup

1. **Board**: Nucleo-F030R8  
2. **UART Pins** (USART1):  
   - `PA9` (MCU TX) → **RX** of your USB-to-Serial adapter  
   - `PA10` (MCU RX) → **TX** of your USB-to-Serial adapter  
   - **GND** → GND  
3. **LED**: User LED on `PA5` (standard for many Nucleo-64 boards).  
4. **Baud**: 115200, 8N1, No flow control.

> **Important**: The onboard ST-Link Virtual COM port on Nucleo-F030R8 typically connects to **USART2** (`PA2`/`PA3`), not USART1. You **must** use an **external USB-to-Serial** adapter wired to `PA9`/`PA10`.

---

## Project Structure

```
.
├── include/
├── lib/
├── src/
│   └── main.c     # Main DMA UART code (this example)
└── platformio.ini
```

---

## Quick Start

1. **Install PlatformIO**: e.g., [VS Code Extension](https://platformio.org/install) or CLI.  
2. **Clone** this repo:
   ```bash
   git clone https://github.com/your_username/stm32f030r8_dma_uart_example.git
   ```
3. **Open** the folder in VS Code or your preferred IDE.
4. **Wire** the USB-to-Serial adapter:
   - `PA9` → RX on adapter
   - `PA10` → TX on adapter
   - Common GND
5. **Build**:
   ```bash
   pio run
   ```
6. **Upload**:
   ```bash
   pio run --target upload
   ```
7. **Open** your USB-to-Serial adapter’s COM port at 115200 8N1 in a terminal application.

---

## Usage

- **Greeting**: You should see:
  ```
  DMA UART Demo: Send 4 bytes to see LED toggle + echo!
  ```
  once at startup.

- **Send 4 Bytes**: Each time you send exactly 4 consecutive bytes in one burst, the LED toggles on `PA5`, and the same 4 bytes are echoed back with a `\r\n` appended.

  - For example, on macOS/Linux:
    ```bash
    echo -n "ABCD" > /dev/tty.usbserial-0001
    ```
    This sends exactly 4 bytes (`A B C D`), no newline. The MCU toggles the LED, then echoes back `ABCD\r\n`.

- **Short Bursts**: Because **DMA in Normal Mode** is set to `RX_SIZE = 4`, it **only** fires the complete callback after exactly 4 bytes arrive.

### Changing `RX_SIZE`
- Adjust `#define RX_SIZE` in `main.c` to a different value (e.g., 8, 32).  
- If you do, you must send that **exact** number of bytes for the callback to trigger.  
- For partial or continuous streaming, consider **DMA Circular Mode**.

---

## Troubleshooting

1. **LED Toggles but No Echo**  
   - If the LED toggles, the RX callback definitely fired, and the code called `HAL_UART_Transmit_DMA()` for your 4 bytes.  
   - Often a **terminal display** issue (short bursts or line endings might not appear).  
   - Try **Hex/Raw Mode** or a capture file to confirm the data is actually arriving.

2. **No Data at All**  
   - Ensure you’re on the correct COM port for the **USB-to-Serial adapter**, not the ST-Link VCP.  
   - Check wiring: `PA9 → adapter RX`, `PA10 → adapter TX`, GND ↔ GND.  
   - Confirm `huart1.Init.BaudRate = 115200` matches your terminal’s baud.

3. **Partial Reception**  
   - If you type slower or send less than 4 bytes, the DMA callback won’t fire. Use a single bulk send or smaller `RX_SIZE`.

4. **Still Nothing?**  
   - Add a **logic analyzer** or second USB-to-Serial adapter on `PA9` to confirm the echo data is physically sent.  
   - Double-check **DMA channels**: `USART1_TX → DMA1_Channel2`, `USART1_RX → DMA1_Channel3`.

---

## License

This project is provided under the [MIT License](LICENSE) (or whichever license you prefer). You are free to use, modify, and distribute it in your own projects.

---

## Contributing

Issues or pull requests are welcome if you have improvements or encounter bugs. This is a minimal example intended to help you learn **DMA-based UART** on the **STM32F030R8**.

---

**Happy Coding!**
