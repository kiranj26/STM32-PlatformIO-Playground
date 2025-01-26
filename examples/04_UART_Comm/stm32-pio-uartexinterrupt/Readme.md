# STM32 Nucleo-F030R8: UART (USART1) Interrupt Example

This repository demonstrates **UART interrupt-based** reception and transmission on an **STM32 Nucleo-F030R8** board using **USART1** (pins `PA9`/`PA10`). It also blinks the user LED (`PA5`) so you can confirm the MCU is running without being blocked by UART operations.

## Serial Terminal    
![alt text](<Screenshot 2025-01-25 at 9.01.38 PM.png>)

## Features

1. **UART RX Interrupt** on `PA10`:
   - A single byte is received via interrupt (`HAL_UART_Receive_IT`).
   - On reception, `HAL_UART_RxCpltCallback()` is called, echoing the byte back.
2. **UART TX Interrupt** on `PA9`:
   - Echo is sent using `HAL_UART_Transmit_IT`, which also returns immediately.
3. **LED Blink** on `PA5` to show non-blocking behavior.
4. **8 MHz** internal HSI clock setup (no PLL). Adjust `SystemClock_Config()` if you want 48 MHz.

## Hardware Requirements

- **Nucleo-F030R8** board  
- **USB-to-Serial Adapter** (FTDI, CP2102, CH340, etc.)
  - Connect:
    - `PA9` (MCU TX) → `RX` on adapter  
    - `PA10` (MCU RX) → `TX` on adapter  
    - Common `GND`  
- **Terminal** on your PC:
  - Baud: `115200`
  - 8 data bits, No parity, 1 stop bit, No flow control

## Project Structure

```
.
├── include/            # optional headers
├── lib/                # optional libraries
├── src/
│   └── main.c          # main application with UART Interrupt code
└── platformio.ini      # PlatformIO configuration
```

## Quick Start

1. **Install PlatformIO** (e.g. in VS Code or via CLI).  
2. **Clone** this repository:
   ```bash
   git clone https://github.com/your_username/uart_interrupt_f030r8.git
   ```
3. **Open** the folder in VS Code (or your preferred environment).
4. **Connect** the Nucleo-F030R8 via USB for power/firmware upload.  
5. **Wire** PA9 → RX, PA10 → TX to your USB-to-Serial adapter, and GND ↔ GND.
6. **Build**:
   ```bash
   pio run
   ```
7. **Upload**:
   ```bash
   pio run --target upload
   ```
8. **Open a Serial Terminal**:
   - Select your USB-to-Serial adapter’s COM port at **115200 8N1**.  
   - You should see `"UART Interrupt Example: Initialized"` once on reset.

## Usage

- **Observe LED**: The user LED on `PA5` toggles every 500 ms.  
- **Type characters** in your serial terminal. Each received character triggers an interrupt and is echoed back.  
- **Non-blocking**: The main loop continues blinking the LED without waiting for UART.

## Customization

- **Baud Rate**: Change `huart1.Init.BaudRate` in `MX_USART1_UART_Init()`.
- **LED Pin**: If your board uses a different LED pin, adjust `MX_GPIO_Init()` accordingly.
- **System Clock**: For higher speeds, set up PLL in `SystemClock_Config()` for 48 MHz. Update flash latency if needed.

## Troubleshooting

1. **No Echo**:
   - Verify wiring (`PA9 → RX`, `PA10 → TX`, GND ↔ GND).
   - Confirm you opened the correct COM port (the **USB-to-Serial adapter**, not the ST-Link VCP).
   - Check `115200 8N1` settings in your terminal.
   - Make sure interrupts are enabled (`HAL_NVIC_EnableIRQ(USART1_IRQn)`).
2. **Partial/Garbled Output**:
   - Check system clock settings or try a lower baud rate to confirm.
   - Ensure your terminal matches the code’s baud (115200).
3. **LED Not Blinking**:
   - If the code is stuck in a `while(1)` due to clock init errors, confirm `SystemClock_Config()` is correct.

## License

This code is provided under the [MIT License](LICENSE) (or whichever license you prefer). You can use, modify, and distribute in your own projects.

## Contributing

Feel free to open issues or PRs. This is a minimal example to demonstrate interrupt-driven UART on STM32F0.

---
**Happy Coding!**
