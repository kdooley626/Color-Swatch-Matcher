# Tech Stack

This describes the system as it exists today.
— proposed changes can be found in `roadmap.md`.

## Architecture overview

```
   Color swatch
        │  (reflected light)
        ▼
  TCS34725 sensor ──► Sensor Arduino ──USB serial──┐
                                                     │
                                              Python CLI (laptop)
                                                     │
   3 peristaltic pumps ◄── Pump Arduino ◄──USB serial┘
        │  (red / yellow / blue dye)
        ▼
     Cup of dyed water
```

Two independent Arduino boards each talk to the laptop over their own USB serial
connection. The laptop is the only coordinator; the two boards never talk to each
other directly.

## Hardware

**Sensing**

- **Adafruit TCS34725** RGB color sensor (I²C), configured for 50 ms integration
  time and 4× gain.
- 3D-printed black enclosure that fixes the swatch-to-sensor distance and seals out
  ambient light to standardize readings.
- One Arduino (the "sensor board") running `ColorSensorTestv2/ColorSensorTestv2.ino`.

**Dispensing**

- Three **peristaltic pumps**, one each for red, yellow, and blue dye solution.
- **L298N** dual H-bridge motor drivers controlling the pumps. Each pump uses a
  speed (PWM) pin plus two direction pins.
- One Arduino (the "pump board") running `ColorMixerv4/ColorMixerv4.ino`.
- 3D-printed structure and mountings for the ingredient bottles, pumps, and the
  electrical enclosure.

**Pump pin map (current wiring, from `ColorMixerv4.ino`)**

| Pump   | Speed (PWM) | Dir 1 | Dir 2 |
| ------ | ----------- | ----- | ----- |
| Red    | 9           | 8     | 7     |
| Yellow | 6           | 3     | 4     |
| Blue   | 5           | 12    | 13    |

**Calibrated pump rates (grams per second)**

| Pump   | Rate  |
| ------ | ----- |
| Red    | 1.612 |
| Yellow | 1.462 |
| Blue   | 1.532 |

These rates convert a target mass into a pump run-time (`time = mass / rate`).

## Firmware (Arduino / C++)

- **Sensor board** (`ColorSensorTestv2.ino`)
  - Library: `Adafruit_TCS34725`, `Wire` (I²C).
  - Reads raw R/G/B/C, normalizes each channel by the clear channel `c`, and
    averages 5 samples per reading.
  - Serial command set: `get sensor status`, `get reading`, `ping`. Replies are
    terminated with a `DONE` line; `ping` replies `PONG`.
  - Final reading line is emitted as plain `r, g, b` for the host to parse.

- **Pump board** (`ColorMixerv4.ino`)
  - Accepts a recipe string `R, Y, B, mass`, normalizes R/Y/B into ratios, scales by
    `mass`, and runs each pump for the computed time.
  - Also supports `ping` → `PONG`. Every command ends with a `DONE` line.
  - Pumps run sequentially with a 500 ms pause between colors.

Both boards run at **9600 baud**.

## Host software (Python)

- **Entry point:** `ColorSwatcherv4.py` — an interactive command-line loop.
- **Dependencies:** `numpy`, `scipy` (`scipy.optimize.nnls`), `pyserial`, plus the
  standard library `time`.
- **Color math:** a fixed `RYB_to_RGB` 3×3 matrix maps an RYB ratio vector to RGB.
  To go the other way, the code solves a **non-negative least squares** problem
  (`nnls`), guaranteeing the RYB amounts are never negative, and reports a relative
  fit error.
- **Serial abstraction:** a `Board` base class wraps a `pyserial` connection with
  `connect` / `disconnect` / `reset_connection` / `send_command` /
  `read_until_done` / `ping`. `SensorBoard` and `PumpBoard` subclass it.
- **Operator commands** (typed at the `Command:` prompt): `get status`,
  `print reading`, `gen recipe`, `duplicate`, `reset`, and `quit`.
- **Safety check:** `PumpBoard` rejects any recipe with non-positive mass or mass
  above a configured maximum (currently 20.0 g).

## Configuration that must be set per machine

- **Serial ports** for both boards are hard-coded near the top of
  `ColorSwatcherv4.py` (lines 141–142). These must be edited to match the actual
  device names (find them via Arduino IDE → Tools → Port).
- **Max pump mass** is a constructor argument to `PumpBoard` (currently `20.0`).
- **Pump rates** and the **`RYB_to_RGB` matrix** are empirically measured constants;
  they are valid only for the current ingredients, tubing, and pumps.

## Repository layout

```
Color-Swatch-Matcher/
├── ColorSwatcherv4.py            # Python host CLI
├── ColorMixerv4/
│   └── ColorMixerv4.ino          # Pump board firmware
├── ColorSensorTestv2/
│   └── ColorSensorTestv2.ino     # Sensor board firmware
├── Project_Summary.pdf           # Project overview & results
├── README.md
└── specs/                        # This constitution
    ├── mission.md
    ├── tech-stack.md
    └── roadmap.md
```

## Known rough edges (carried into the roadmap)

- A typo in `Board.connect` calls `self.serial.cose()` instead of `close()`.
- `Board.ping(self, command)` takes an unused `command` arg but is called with none.
- Serial ports are hard-coded rather than configurable.
- Calibration constants live inline in source

Note: This tech-stack file was generated with the assistance of AI (based on inputs such as codes and project summaries written by author)
