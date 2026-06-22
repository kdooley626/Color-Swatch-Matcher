# Roadmap

## Phase 0 — Baseline (already done)

The current, working prototype. This is the starting point, captured here so later
phases have something to diff against.

- [x] Sensor board reads averaged, normalized RGB from the TCS34725.
- [x] Pump board dispenses an `R, Y, B, mass` recipe across three pumps.
- [x] Python CLI converts RGB → RYB via non-negative least squares and drives both
      boards.
- [x] Four sample colors duplicated successfully (qualitatively) in testing.
- [x] Documented calibration procedure

---

## Phase 1 — Stabilize what exists

Small bug fixes only. No new features. Goal: make the current behavior reliable.

- [ ] Fix the `self.serial.cose()` typo in `Board.connect` → `self.serial.close()`.
- [ ] Fix `Board.ping` signature: drop the unused `command` argument so calls match.
- [ ] Confirm `gen recipe` / `cmd_pump` end-to-end (it sends a raw recipe to the
      pumps) and correct the command-name/label mismatch if found.
- [ ] Verify graceful handling when a board is unplugged or the wrong port is set
      (clear error, no crash).

## Phase 2 — Make configuration external

Stop editing source code to run the robot.

- [ ] Move the two serial port names out of source into a small config file
      (e.g. `config.json` or a `.env`), with the current values as defaults.
- [ ] Move `max_mass` and the pump rates into the same config.
- [ ] Update `README.md` so setup is "edit config, run script" instead of "edit
      lines 141–142".

## Phase 4 — Operator experience polish

Make the CLI easier and harder to misuse. No hardware changes.

- [ ] Show the computed RYB ratio and relative error to the operator _before_
      pumping, with a confirm step on `duplicate`.
- [ ] Normalize / unify command names and add a `help` command listing them.
- [ ] Add an input guard so a `duplicate` with no valid sensor reading aborts
      cleanly (it partly does this already — make it explicit).

## Phase 5 — Logging and repeatability

Be able to answer "what did the robot do?" after the fact.

- [ ] Log each run (timestamp, RGB read, RYB recipe, mass, relative error) to a CSV.
- [ ] Add a "repeat last recipe" command so a good mix can be reproduced exactly.

## Phase 6 — Reading quality improvements

Tighten the sensing side, still open-loop.

- [ ] Make the sample count (currently 5) configurable and report variance across
      samples so a noisy reading is visible.
- [ ] Add a white/reference calibration step to reduce sensitivity to sensor drift
      and enclosure lighting.

## Phase 7 — Validate the match numerically

Move from "looks close" toward a measured match quality.

- [ ] Read the _dispensed_ result (dry swatch of the duplicate) and compute a color
      difference (e.g. ΔE) against the original.
- [ ] Use those measurements to test the open-loop accuracy claim and to spot which
      colors the three ingredients handle worst (the project already flags orange).

## Phase 8 — Stretch: closed-loop and more ingredients

Explicitly future work, only after everything above is solid. Each is its own small
project and should be scoped further when reached.

- [ ] Investigate reading color in _liquid_ form to enable a closed-loop correction
      pass (the key blocker noted in the project summary: samples currently must dry
      before reading).
- [ ] Add a fourth ingredient (e.g. white or black) and extend the least-squares fit
      to more than three channels.
- [ ] Optional: replace the CLI with a simple GUI once the workflow is stable.

---

## How to use this roadmap

- Pick the lowest-numbered unchecked item.
- Make the change, test it on the bench, check the box.
- If a phase grows too big, split it into smaller items rather than starting the
  next phase early.
- Keep `mission.md` and `tech-stack.md` in sync as reality changes.
