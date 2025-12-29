# CD4066 / 74HC4066 Quad Bilateral Switch (Wokwi Custom Chip)

This repo provides a **Wokwi Custom Chip** that behaves like a **CD4066 / 74HC4066**: four independent, bidirectional “analog switch” channels.

It’s mainly useful for simulating things like:
- button presses (shorting two pads/nets)
- simple signal gating
- “electronic contact closure” behavior without using relays in your diagram

## What it simulates

Each channel has three pins: **Y**, **Z**, **E**.

- When **E = LOW**: the channel is **open** (Y and Z are not connected).
- When **E = HIGH**: the channel is **closed** (Y and Z are treated like they’re shorted together).

This is an **idealized model** intended for digital logic and “button press” style circuits.
It does **not** try to accurately model on-resistance, leakage, bandwidth, charge injection, etc.

## Pinout (DIP-14)

Pin numbering matches the common DIP-14 package:

| Pin | Name |
|-----|------|
| 1   | 1Y   |
| 2   | 1Z   |
| 3   | 1E   |
| 4   | 2Z   |
| 5   | 2Y   |
| 6   | 2E   |
| 7   | GND  |
| 8   | 3E   |
| 9   | 3Y   |
| 10  | 3Z   |
| 11  | 4E   |
| 12  | 4Z   |
| 13  | 4Y   |
| 14  | VCC  |

> Note: `VCC` and `GND` are present for diagram correctness.  
> The current behavioral model doesn’t depend on supply voltage.

## Quick start (use in your Wokwi project)

### 1) Get the built chip files
Download the latest build output from one of these:
- **GitHub Releases** (recommended), or
- **Actions artifacts** from a successful workflow run

You want two files:
- `*.chip.wasm`
- `*.chip.json`

Place them in your project, for example:
chips/
cd4066.chip.wasm
cd4066.chip.json



### 2) Register the chip in `wokwi.toml`

```toml
[[chip]]
name = "cd4066"
binary = "chips/cd4066.chip.wasm"
```

### 3) Use it in diagram.json
```json

{
  "parts": [
    { "type": "wokwi-esp32-devkit-v1", "id": "esp", "top": 60, "left": -160, "attrs": {} },
    { "type": "chip-cd4066", "id": "sw", "top": 60, "left": 120, "attrs": {} }
  ],
  "connections": [
    ["esp:3V3",  "sw:VCC", "red",   []],
    ["esp:GND.1","sw:GND", "black", []],

    ["esp:D18", "sw:1E", "green", []],
    ["sw:1Y",   "BTN1_A", "blue", []],
    ["sw:1Z",   "BTN1_B", "blue", []]

    // repeat for channels 2–4 as needed
  ]
}
```
In plain English:

Connect Y/Z to the two nets you want to short (like a real button’s two pads).

Drive E from a GPIO.

Pulse E HIGH for ~100–200ms to simulate a press.

Recommended wiring patterns
“Button press” pattern (common)
If one side of your button is a scan/pullup line and the other is ground/low:

Put Y on the scan side

Put Z on the low side

Closing the switch pulls the scan line low (press detected)

Avoid ghost presses
If you’re driving E from a microcontroller, add:

1k series resistor from GPIO → E (optional but nice)

100k pulldown from E → GND (recommended) so it stays OFF during boot

Building (dev notes)
This repo is meant to be built by CI (GitHub Actions) to produce the .chip.wasm output.
If you’re editing behavior/pin definitions:

update the chip .json pin list

update the C implementation

push and let CI build artifacts/releases

Limitations
Idealized “short” behavior. Great for digital/button use.

Not a precision analog simulator.

If both sides drive conflicting values, the model prioritizes “LOW wins” behavior (useful for pullup-to-ground button press simulation).

License
MIT (or whatever you’ve chosen for the repo).

Credits
Built on Wokwi Custom Chips (C API + WASM compilation flow).