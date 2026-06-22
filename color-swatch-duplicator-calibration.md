# Color Swatch Machine Calibration Procedure

## Overview

Machine calibration consists of two steps:

1. **Pump Speed Calibration** – Calibrate the pump flow rates so the machine accurately dispenses the desired mass of each ingredient color. This must be performed whenever ingredient mixtures are refilled because the dye-to-water ratio can change the fluid density.

2. **Ingredient RGB Calibration** – Measure the RGB values of the individual red, yellow, and blue ingredients and update the RYB-to-RGB conversion matrix used by the software. This must also be performed whenver ingredient mixtures are refilled or a new base paper is used because every mixture and paper will give slightly different RGB values.

---

# Pump Speed Calibration

### 1. Generate a Single-Color Recipe

Run the ColorSwatcher Python script and enter the command:

```text
gen recipe
```

### 2. Prepare a Scale

- Place an empty cup on a mass scale.
- Tare (zero) the scale.

### 3. Dispense a Known Mass

Generate a recipe containing only one ingredient color.

For example, to dispense only blue:

```text
0,0,1,m
```

where:

- `m` = desired mass to dispense (grams)

### 4. Measure Actual Output

After dispensing is complete, record the actual mass of liquid in the cup.

### 5. Calculate Updated Pump Flow Rate

Use the existing pump flow rate and the measured output mass to calculate an updated flow rate:

[
r_i = (m_actual/m_input) * r_0
]

where:

| Variable   | Description              |
| ---------- | ------------------------ |
| (r_i)      | New pump flow rate       |
| (r_0)      | Existing pump flow rate  |
| (m_actual) | Measured dispensed mass  |
| (m_input)  | Commanded dispensed mass |

### 6. Update Arduino Code

Update the pump flow rate constants in the **ColorMixerv4** Arduino script (Lines 35–37).

### 7. Verify Calibration

Run the pump again using a known mass command and measure the output.

If the measured mass is outside the acceptable error range:

- Repeat Steps 1–6.

### Acceptance Criteria

A pump is considered calibrated when it dispenses the desired mass within the specified error tolerance **twice consecutively**.

---

# Ingredient RGB Calibration

### 1. Dispense Ingredient Samples

For each ingredient color:

- Red
- Yellow
- Blue

Use the **generate recipe** command to dispense at least **7 grams** into separate sample cups.

### 2. Prepare Color Samples

For each ingredient:

1. Cut a piece of watercolor paper sized to fit inside the sensor box.
2. Dip the paper into the ingredient mixture for approximately **3 seconds**.
3. Ensure the sample is completely saturated.

### 3. Dry Samples

Place samples on a paper towel and allow them to dry completely.

Recommended drying time:

- Two hours minimum
- Overnight preferred

### 4. Measure RGB Values

Once dry:

1. Insert a sample into the sensor box.
2. Run the command:

```text
get reading
```

3. Record the RGB reading.

Repeat for all three ingredient colors.

### 5. Update the RYB-to-RGB Matrix

Populate the matrix columns using the measured RGB values:

| Matrix Column | Ingredient                    |
| ------------- | ----------------------------- |
| Column 1      | Red ingredient RGB reading    |
| Column 2      | Yellow ingredient RGB reading |
| Column 3      | Blue ingredient RGB reading   |

Example:

```text
RYB_to_RGB =
[
    [R_red,    R_yellow,    R_blue],
    [G_red,    G_yellow,    G_blue],
    [B_red,    B_yellow,    B_blue]
]
```

### 6. Update Python Script

Replace the existing matrix values in **ColorSwatcherv4.py** (Lines 7–9) with the newly measured RGB values.

---

# Calibration Complete

The machine is fully calibrated when:

- All pumps dispense accurate masses within the desired tolerance.
- The RYB-to-RGB matrix has been updated using fresh ingredient measurements.

After calibration, the Color Swatch Machine is ready for normal operation.
