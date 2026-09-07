# speed_controller (Simulink model description)

This file stands in for the `speed_controller.slx` Simulink model. A
`.slx` file is a binary MATLAB/Simulink project file, and no MATLAB/Simulink
toolchain is available in this repository (or in most CI runners), so it
cannot be committed and regenerated here the way `../src/` (this module's
own source folder) normally would be. This document instead describes,
in words, the block diagram that `../src/speed_controller.c` corresponds
to, and the generated C sources were hand-authored to faithfully match
the file and comment structure Simulink Embedded Coder actually emits
(banner comments, `ExtU_`/`ExtY_`/`DW_`/`P_` structs, `<model>_initialize`
/ `_step` / `_terminate` entry points) -- they are **not** real Embedded
Coder output. See the repository's top-level `README.md` for why this
module is traced differently from the other six (hand-written) modules.

## Block diagram (`<Root>`)

```
Error ──▶ [Kp Gain] ──▶ [Sum] ──▶ [Saturation] ──▶ Command
            (2.0)          ▲       [-100, 100]
                            │
Error ──▶ [Ki Gain] ──▶ [Integrator]
            (0.5)      (discrete, dt = SampleTime)
```

- `Kp` (Gain block): proportional gain, mask parameter `Kp = 2.0`.
- `Ki` (Gain block) + `Integrator` (Discrete-Time Integrator block):
  integral term, mask parameters `Ki = 0.5`, `SampleTime = 0.1`.
- `Sum`: adds the proportional and integral terms.
- `Saturation`: clamps the combined signal to `[LowerLimit, UpperLimit]`
  = `[-100.0, 100.0]` before it leaves the model as `Command`.

## Tunable parameters (Simulink mask / `P_speed_controller_T`)

| Parameter | Value | Generated field |
|---|---|---|
| `Kp` | 2.0 | `speed_controller_P.Kp_Gain` |
| `Ki` | 0.5 | `speed_controller_P.Ki_Gain` |
| `SampleTime` | 0.1 | `speed_controller_P.SampleTime` |
| `UpperLimit` | 100.0 | `speed_controller_P.Saturation_UpperSat` |
| `LowerLimit` | -100.0 | `speed_controller_P.Saturation_LowerSat` |

## Traceability

- REQ-7 / ARCH-7 (`../docs/architecture.sdoc`,
  `../docs_needs/architecture.rst`, both in this module's own folder)
  reference `../src/speed_controller.c` and `.h` directly via a
  **doc-side `TYPE: File` relation**, not an in-file `@relation(...)`
  marker -- the generated files must not be hand-edited, so nothing
  should be added to them by hand, including traceability comments.
  `../tests/speed_controller_test.cpp`, which *is* hand-written, still
  carries the usual `@relation(ARCH-7, ..., role=Test)` marker.
