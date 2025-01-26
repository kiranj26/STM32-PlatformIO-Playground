# STM32F0: Ring Buffer UART Example (Line Commands)

This repository demonstrates an **interrupt-driven UART** with a **ring buffer** for inbound data on an **STM32F030R8** (Nucleo-F030R8). It parses **line-based commands** (`help`, `led on`, etc.) in the main loop.


## Serial Terminal Output
![alt text](<Screenshot 2025-01-26 at 1.18.10 AM.png>)
## Features

1. **Ring Buffer**: Single-byte interrupts store incoming data in `rxRingBuf[]`.
2. **Line Parser**: Accumulates chars until `\r` or `\n`, then processes a command:
   - `help`
   - `led on` / `led off`
   - `ping`
   - `version`
   - unknown => “Unknown command”
3. **User LED** on **PA5** toggles with `led on/off`.
4. **Non-blocking**: The main loop is free to do other tasks.

## Hardware Setup

- **Nucleo-F030R8** board
- **USART1** on pins `PA9 (TX)` / `PA10 (RX)`, 115200 baud, 8N1
- **USB-to-Serial Adapter**:
  - `PA9` → adapter RX
  - `PA10` → adapter TX
  - GND → GND

> **Note**: The onboard ST-Link Virtual COM typically uses **USART2** (`PA2/PA3`). This code uses **USART1**, so you must use an **external USB-to-Serial** adapter for PA9/PA10.

## Project Structure

```
.
├── include/
├── lib/
├── src/
│   └── main.c         # The ring buffer code
└── platformio.ini
```

## Quick Start

1. **Install PlatformIO** (VS Code or CLI).
2. **Clone** this repository:
   ```bash
   git clone https://github.com/your_username/ringbuf_uart_f030r8.git
   ```
3. **Open** in PlatformIO.
4. **Build & Upload**:
   ```bash
   pio run
   pio run --target upload
   ```
5. **Open** the USB-to-Serial adapter’s COM port at `115200 8N1`.
6. On reset, you see:
   ```
   Ring Buffer UART Example
   Type commands: help, led on, led off, ping, version
   ```
7. **Type** a command, e.g. `help` + Enter, and see responses.

## Usage

- **`help`** → lists commands
- **`led on`** → turns LED (PA5) ON, prints “LED ON”
- **`led off`** → turns LED OFF, prints “LED OFF”
- **`ping`** → prints “pong”
- **`version`** → prints “v1.0.0”
- **others** → “Unknown command”

## Troubleshooting

1. **No Data**: 
   - Check you’re on the external adapter’s COM port (not ST-Link VCP).
   - Confirm PA9 → RX, PA10 → TX, GND ↔ GND.
2. **LED**:
   - Make sure your board’s user LED is actually on PA5 (most Nucleo-64 boards).
3. **Buffer Overflow**:
   - If you type >63 chars without pressing Enter, it resets your line and prints “Command too long, reset.” Increase `CMDLINE_SIZE` if needed.

## License

MIT (or whichever license you prefer). Feel free to adapt or integrate into your own projects.

## Contributing

Open PRs or issues for improvements, e.g. advanced parser, DMA-based TX, etc.

**Enjoy your ring buffer-based UART command application!**
