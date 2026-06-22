# Mission

## Overview

The **Color Swatch Duplicator** is an electromechanical robot that reads a physical
color swatch and attempts to reproduce that color as a dyed-water solution. The
reproduced liquid is dispensed into a cup and can be painted onto watercolor paper,
where it can be compared side-by-side with the original.

## Primary Objective

Given a color the robot can see, it will mix the closest possible match using only
the liquid ingredients on hand (red, yellow, and blue dye dissolved in water). It
is trying to replicate the sample color as closely as possible with the available ingredients.
Theoretically the ingredients could be any threee colors, as long as the robot is properly calibrated
following the written procedure. Primary colors were chosen in an attempt to maximize the range of possible
colors that the robot can produce.

## How it works

1. A TCS34725 RGB sensor, mounted in a 3D-printed black enclosure that fixes the
   sample distance and blocks ambient light, reads a dried swatch, and reports a
   normalized RGB value.
2. A Python program converts that RGB value into a Red/Yellow/Blue mixing ratio
   using a non-negative least-squares fit against a measured RYB-to-RGB matrix.
3. A second Arduino drives three peristaltic pumps (via L298N motor drivers) to
   dispense the ingredients in the computed ratio, scaled to a user-specified total
   mass.

The two Arduinos (one for sensing, one for pumping) are coordinated by a single
Python command-line interface running on the operator's laptop.

## Scope and boundaries

- **Three ingredients only.** Currently working with: Red, yellow, and blue dye in water. The match quality
  is bounded by what these three can produce.
- **Open-loop control.** The robot does not currently verify what it dispensed.
  Swatches must dry before they can be read, so there is no real-time feedback
  during a mix. The operator judges the result by eye, but they can also take a
  reading on the new sample and compare to the old manually.
- **Single operator, single laptop.** Control is a local CLI over USB serial.
- **Qualitative success.** Working to produce samples appearing close in shade to the viewer.
  The least-squares fit reports a relative error, but the project does not yet claim a calibrated, numeric
  color-accuracy guarantee.

## What success looks like

A reliable setup that can be calibrated with any three liquid ingredients (able to pass through peristaltic pumps)
and attempt to replicate any color sample as close as possible.

## Guiding principles

- **Honest about limits.** The robot makes the _closest achievable_ match, not a
  perfect one, and the interface should never imply otherwise.
- **Repeatable readings.** Standardized sensing conditions (fixed
  distance, sealed enclosure, averaged samples).
- **Safe by default.** Pumps never run without a valid, bounded recipe; mass is
  always checked against a configured maximum before any pump turns.
- **Small, verifiable steps.** Changes are made in small increments that can be
  tested on the bench before moving on (see `roadmap.md`).

Note: This mission structure was generated with the assistance of AI and refined by the developer to match project goals
