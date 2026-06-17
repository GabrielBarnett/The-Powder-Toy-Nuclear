# Stage 3 FISS Radiation And Energy Transport Design Spec

## Source And Status

This spec is derived from `stage_3_design.txt` and assumes Stage 1A, Stage 1B, and Stage 2 are present on `master`.

Current system status:
- `FISS` stores fictional compression in `tmp`.
- `FISS` stores fictional activation in `tmp2`.
- Compressed `FISS` can emit `NTRN`.
- Activated and compressed `FISS` can fission into `FPRD`.
- Fission currently produces heat, pressure, `NTRN`, optional `SHOK`, and nearby `FISS` chain-kicks.
- Stage 2 added pressure tools and diagnostics: `CMTR`, `HEXP`, `SHOK`, `DENS`, `BEXP`, `SEXP`, `PABS`, `PWAL`, and `PDRV`.

Stage 3 must preserve the successful compression-driven `FISS` core. Do not redesign the core fission thresholds, compression mechanics, or pressure tools unless required for integration stability.

## Safety And Scope

This is a fictional particle sandbox mechanic only. All values must be arbitrary gameplay constants.

Do not add or model:
- real-world nuclear weapon parameters,
- real isotope data,
- real radiation constants,
- real shielding tables,
- real dose units,
- real neutron cross-sections,
- real explosive data,
- real weapon geometry,
- critical mass, tampers, reflectors, fusion staging, or yield calculations,
- real thermal pulse values,
- real blast timing,
- real material burn thresholds,
- fusion fuels, lithium materials, deuterium/tritium materials, moderators, or neutron absorbers.

Avoid terminology and implementation details that imply real nuclear engineering. `HRAY`, `XRAY`, `RADS`, and `IONZ` are abstract gameplay effects for heat, energy transport, debris, and visuals.

`GAMM` is explicitly out of scope for Stage 3. It may be reconsidered only after the core Stage 3 system is stable.

## Design Goals

1. Add fast fictional thermal/radiation transport so fission reactions are not only local heat, pressure, and `NTRN`.
2. Let exposed distant materials heat or scorch before slower pressure and `SHOK` effects arrive.
3. Add a visible radiation-flash aftermath through `XRAY`, `IONZ`, and `RADS`.
4. Keep `NTRN` as the main chain/fission driver.
5. Keep `FISS` as the main high-energy source.
6. Make radiation behavior easy to debug and tune through `RMTR`.
7. Enforce caps so reactions are dramatic without unlimited particle creation, runaway emissions, or major FPS collapse.

## Recommended Approach

Implement Stage 3 as one bounded transport layer:

1. Add Stage 3 tuning constants and caps to the existing `FISS.h` tuning area.
2. Add frame caps for `HRAY`, `XRAY`, `RADS`, `IONZ`, thermal flash events, and low-level `RADS` emissions.
3. Add helper functions for safe capped creation of `HRAY`, `XRAY`, `RADS`, and `IONZ`.
4. Add `ApplyThermalFlash` as an immediate abstract line-of-sight heat helper.
5. Add elements `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR`.
6. Integrate Stage 3 emissions into `PerformFissionEvent` in the required ordering.
7. Add low-level radiation behavior to `FPRD` and `RADS`.
8. Compile after each major slice.
9. Run source-level safety checks and manual sandbox tests.

## Tuning Organization

Keep constants in `src/simulation/elements/FISS.h` unless the implementation already needs a broader tuning header. Group Stage 3 constants separately:

- thermal flash constants,
- `HRAY` constants,
- `XRAY` constants,
- `RADS` constants,
- `IONZ` constants,
- `RMTR` constants,
- fission-emission constants,
- `FPRD` Stage 3 decay/emission constants,
- per-frame caps.

All values must be named plainly and tuned as gameplay values, not physical constants.

## New Elements

### HRAY

`HRAY` is a fictional heat-ray / thermal-radiation particle.

Purpose:
- Represents fast radiant heat from a `FISS` reaction.
- Travels much faster than `SHOK`.
- Passes through air and gas easily.
- Heats or scorches exposed solids and liquids when absorbed.
- May create small amounts of `IONZ` as a visual trail.
- Does not add pressure.
- Does not emit `NTRN`.
- Does not directly trigger fission except through ordinary heat effects.

Required behavior:
- Fast-moving particle with short life.
- Emitted with random or outward velocity from `FISS`.
- When crossing empty air or gas, usually continues.
- When hitting solids or liquids, may deposit heat and disappear.
- May penetrate some materials with a tunable arbitrary chance.
- May create `IONZ` with a capped low chance.
- Must obey `MAX_HRAY_CREATED_PER_FRAME`.
- Must not create unlimited particles or recursive emissions.

Required constants:
- `HRAY_LIFE_MIN`
- `HRAY_LIFE_MAX`
- `HRAY_SPEED_MIN`
- `HRAY_SPEED_MAX`
- `HRAY_HEAT_DEPOSIT`
- `HRAY_SURFACE_HEAT_MULTIPLIER`
- `HRAY_ABSORPTION_CHANCE`
- `HRAY_PENETRATION_CHANCE`
- `HRAY_IONZ_CHANCE`
- `MAX_HRAY_CREATED_PER_FRAME`

Visual design:
- Use a warm orange/white thermal color.
- Must be visually distinct from `XRAY`, `NTRN`, and `SHOK`.

### XRAY

`XRAY` is a fictional penetrating energy/radiation particle.

Purpose:
- Adds fast radiation-like energy transport without simply increasing heat and pressure everywhere.
- Travels through air and some particles.
- Deposits small or moderate heat when absorbed.
- Creates `IONZ` in air/gas as a visible radiation-flash effect.
- May slightly activate or disturb nearby `FISS`, but must not replace `NTRN` as the main fission driver.

Required behavior:
- Fast-moving particle with short life.
- Emitted with random or outward velocity from `FISS`, `FPRD`, and `RADS`.
- May pass through particles by arbitrary chance.
- May be absorbed, depositing heat and disappearing.
- May create `IONZ` while travelling through air/gas.
- May add a small `FISS.tmp2` activation bonus if it intersects or passes near `FISS`.
- Must obey `MAX_XRAY_CREATED_PER_FRAME`.
- Must not create unlimited `IONZ` or secondary radiation loops.

Required constants:
- `XRAY_LIFE_MIN`
- `XRAY_LIFE_MAX`
- `XRAY_SPEED_MIN`
- `XRAY_SPEED_MAX`
- `XRAY_HEAT_DEPOSIT`
- `XRAY_IONZ_CHANCE`
- `XRAY_ABSORPTION_CHANCE`
- `XRAY_PENETRATION_CHANCE`
- `XRAY_FISS_ACTIVATION_BONUS`
- `MAX_XRAY_CREATED_PER_FRAME`

Visual design:
- Use a cool blue/white or violet energy color.
- Must be visually distinct from `HRAY`, `NTRN`, and `SHOK`.

### RADS

`RADS` is fictional radioactive-looking debris/contamination.

Purpose:
- Adds lingering post-fission debris.
- Gives `FPRD` a Stage 3 decay path.
- Provides low-level ongoing radiation-like activity after a `FISS` reaction.
- Eventually decays into an inert existing byproduct such as `BCOL`, `BRCK`, `DUST`, or `SMKE`, depending on the easiest stable fit.

Required behavior:
- Produced by `FISS` fission and/or `FPRD` decay.
- Starts warm or hot.
- Cools over time.
- Has finite `life`.
- Occasionally emits weak `XRAY` with low probability.
- Very rarely emits `NTRN` if this stays stable and capped.
- May create nearby `IONZ` with low probability.
- Eventually converts to an inert harmless byproduct.
- Must not become the main source of runaway reactions.
- Must obey `MAX_RADS_CREATED_PER_FRAME` and `MAX_RADS_EMISSIONS_PER_FRAME`.

Required constants:
- `RADS_START_TEMP`
- `RADS_START_LIFE`
- `RADS_COOLING_RATE`
- `RADS_XRAY_EMISSION_CHANCE`
- `RADS_NTRN_EMISSION_CHANCE`
- `RADS_IONZ_CHANCE`
- `RADS_DECAY_TIME`
- `MAX_RADS_CREATED_PER_FRAME`
- `MAX_RADS_EMISSIONS_PER_FRAME`

Visual design:
- Use a muted green/yellow or dirty hot residue color.
- Must look like lingering debris, not the main fission flash.

### IONZ

`IONZ` is fictional ionised/excited gas.

Purpose:
- Shows where `HRAY`, `XRAY`, and `RADS` energy passed through air/gas.
- Makes fission flashes and radiation paths visible.
- Acts as a short-lived glowing gas/energy effect.

Required behavior:
- Short-lived gas or energy-like effect.
- Glows and fades quickly.
- Adds only modest heat by itself.
- Slowly cools.
- Does not emit `NTRN`.
- Does not directly trigger `FISS`, except through ordinary heat if local conditions happen to allow it.
- May become existing plasma/fire only through normal temperature behavior if extremely hot.
- Must obey `MAX_IONZ_CREATED_PER_FRAME`.

Required constants:
- `IONZ_START_TEMP`
- `IONZ_LIFE`
- `IONZ_COOLING_RATE`
- `IONZ_FADE_RATE`
- `IONZ_PLASMA_TEMP_THRESHOLD`
- `MAX_IONZ_CREATED_PER_FRAME`

Visual design:
- Use a glowing cyan/violet or electric color.
- Must not look identical to `PLSM`, `XRAY`, or `SHOK`.

### RMTR

`RMTR` is the Stage 3 radiation diagnostic meter.

Purpose:
- Helps tune and debug `HRAY`, `XRAY`, `RADS`, `IONZ`, and thermal/radiation activity.
- Mirrors `CMTR` philosophy but measures radiation/energy products instead of compression and activation.

Required baseline behavior:
- Samples nearby `HRAY`, `XRAY`, `RADS`, and `IONZ`.
- Stores or displays radiation intensity using `tmp`, `tmp2`, `life`, `temp`, color, or another local convention.
- Does not meaningfully react.
- Does not emit radiation.

Preferred mode support using `ctype`:
- Mode 0: nearby `HRAY` intensity.
- Mode 1: nearby `XRAY` intensity.
- Mode 2: nearby `RADS` intensity.
- Mode 3: nearby `IONZ` intensity.
- Mode 4: combined radiation/thermal intensity.
- Mode 5: recent radiation-hit memory, if practical.

If full mode support creates too much complexity, implement the combined mode first and keep `ctype` modes as a follow-up.

Required constants:
- `RMTR_SAMPLE_RADIUS`
- `RMTR_DECAY_RATE`
- `RMTR_MAX_SIGNAL`

## Thermal Flash Helper

Add a helper:

```cpp
void ApplyThermalFlash(Simulation *sim, int x, int y, int radius, float intensity);
```

Purpose:
- Immediately applies an abstract line-of-sight thermal flash around a `FISS` fission event.
- Allows exposed surfaces to heat before `SHOK` or pressure arrives.
- Adds heat only.
- Does not create pressure.
- Does not create `NTRN`.
- Does not create unlimited particles.

Required behavior:
- Scan particles/cells within `THERMAL_FLASH_RADIUS`.
- Prefer line-of-sight exposure if practical.
- Air and gas should block little or nothing.
- Solids should absorb/block the flash.
- Exposed solid or liquid surfaces receive heat.
- Heat falls off with distance using an arbitrary gameplay falloff.
- Heat addition is clamped by `THERMAL_FLASH_MAX_HEAT`.
- Obey `MAX_THERMAL_FLASH_EVENTS_PER_FRAME`.
- If exact line-of-sight is expensive, use a bounded ray step count or a simplified exposure approximation.

Required constants:
- `THERMAL_FLASH_ENABLED`
- `THERMAL_FLASH_RADIUS`
- `THERMAL_FLASH_INTENSITY`
- `THERMAL_FLASH_SURFACE_BONUS`
- `THERMAL_FLASH_DISTANCE_FALLOFF`
- `THERMAL_FLASH_MAX_HEAT`
- `THERMAL_FLASH_LINE_OF_SIGHT_STEPS`
- `MAX_THERMAL_FLASH_EVENTS_PER_FRAME`

## Integration With FISS

Update the existing `PerformFissionEvent` flow. The Stage 3 ordering is important:

1. Check global fission cap.
2. Convert `FISS` to `FPRD`.
3. Apply thermal flash immediately if enabled.
4. Emit `HRAY`.
5. Emit `XRAY`.
6. Emit `NTRN`.
7. Create optional `RADS` debris.
8. Create optional `IONZ` in nearby air/gas.
9. Add ordinary local heat.
10. Add pressure.
11. Spawn optional `SHOK`.
12. Apply local chain-kick to nearby `FISS`.
13. Obey all particle, heat, pressure, and frame caps.

This ordering makes fast thermal/radiation effects visible before slower compression and pressure behavior.

Required fission-emission constants:
- `FISSION_HRAY_CHANCE`
- `FISSION_HRAY_MIN`
- `FISSION_HRAY_MAX`
- `FISSION_HRAY_RADIUS`
- `FISSION_XRAY_CHANCE`
- `FISSION_XRAY_MIN`
- `FISSION_XRAY_MAX`
- `FISSION_XRAY_RADIUS`
- `FISSION_RADS_CHANCE`
- `FISSION_RADS_MIN`
- `FISSION_RADS_MAX`
- `FISSION_IONZ_CHANCE`
- `FISSION_IONZ_RADIUS`

Fission role separation:
- `FISS` remains the main high-energy event.
- `HRAY` and thermal flash handle fast distant heating.
- `XRAY` handles radiation-like energy transport and `IONZ`.
- `NTRN` remains the main chain driver.
- `SHOK` and pressure remain slower compression/blast effects.
- `RADS` and `FPRD` provide aftermath, not a second runaway engine.

## Integration With FPRD

`FPRD` should remain hot debris but gain low-level Stage 3 behavior.

Required behavior:
- Remains hot initially.
- Cools over time.
- Occasionally emits `XRAY` at low probability.
- Very rarely emits `NTRN` only if already stable with current `FPRD` behavior.
- Sometimes produces `RADS` as it decays.
- Eventually becomes inert ash/debris if practical.
- Must not create runaway reactions by itself.

Required constants:
- `FPRD_XRAY_EMISSION_CHANCE`
- `FPRD_RADS_DECAY_CHANCE`
- `FPRD_TO_RADS_CHANCE`
- `FPRD_TO_ASH_TIME`

Implementation note:
- Keep `FPRD` emissions much weaker than direct `FISS` fission emissions.
- If `FPRD` already has a weak `NTRN` emission path, preserve it and add caps rather than increasing it.

## Stability And Caps

Add or enforce:
- `MAX_HRAY_CREATED_PER_FRAME`
- `MAX_XRAY_CREATED_PER_FRAME`
- `MAX_RADS_CREATED_PER_FRAME`
- `MAX_IONZ_CREATED_PER_FRAME`
- `MAX_RADS_EMISSIONS_PER_FRAME`
- `MAX_THERMAL_FLASH_EVENTS_PER_FRAME`
- `MAX_TOTAL_RADIATION_PARTICLES_PER_FRAME`, if useful after individual caps exist.

Required safeguards:
- No infinite `HRAY`, `XRAY`, `RADS`, or `IONZ` loops.
- `RADS` must not recursively create too much `RADS`.
- `XRAY` must not create unlimited `IONZ`.
- `HRAY` must not create unlimited `IONZ`.
- `FPRD` must not create unlimited radiation.
- Thermal flash must not overheat the entire map.
- All neighbor and ray checks must be bounds-safe.
- All particle creation must handle failure safely.
- A compact `FISS` reaction must not cause major FPS collapse.

## Manual Tests

### Test 1: HRAY Manual Spawn

Spawn `HRAY` in open air.

Expected:
- Very fast movement.
- Short life.
- No pressure.
- Possible faint `IONZ` trail.

### Test 2: HRAY Hitting Solid

Fire `HRAY` at a solid material.

Expected:
- Heat deposit or scorching at the exposed surface.
- `HRAY` disappears or weakens.

### Test 3: Thermal Flash Before Shock

Place a `FISS` reaction source and a heat-sensitive object some distance away. Trigger `FISS`.

Expected:
- Distant exposed object heats or scorches before `SHOK` or pressure reaches it.

### Test 4: Blocked Thermal Flash

Put a solid wall between `FISS` and the target. Trigger `FISS`.

Expected:
- Wall absorbs most thermal flash.
- Object behind the wall receives much less heat.

### Test 5: XRAY Manual Spawn

Spawn `XRAY` in open air.

Expected:
- Fast movement.
- Short life.
- Possible `IONZ` trail.
- No runaway.

### Test 6: XRAY Absorption

Fire `XRAY` through common materials.

Expected:
- Some pass-through.
- Some absorption.
- Small or moderate heat deposit.

### Test 7: FISS Radiation Flash

Trigger a compact `FISS` reaction.

Expected:
- `FISS` produces `HRAY`, `XRAY`, and `IONZ` flash in addition to pressure, heat, `NTRN`, and `FPRD`.

### Test 8: RADS Aftermath

Trigger a `FISS` reaction and observe debris.

Expected:
- `RADS` and `FPRD` remain temporarily active.
- Debris cools and decays.

### Test 9: IONZ Behavior

Create `IONZ` via `HRAY`, `XRAY`, or a `FISS` reaction.

Expected:
- Visible short-lived excited gas effect.
- Modest heat.
- Fades quickly.

### Test 10: RMTR

Place `RMTR` near a `FISS` reaction.

Expected:
- `RMTR` indicates nearby radiation/thermal activity.

### Test 11: No Radiation From Pressure Tools Alone

Detonate `HEXP`, `BEXP`, or `SEXP` without `FISS`.

Expected:
- No `HRAY`, `XRAY`, `RADS`, or `IONZ` products from pressure tools alone.

### Test 12: Stability

Build a moderate compressed `FISS` setup and trigger it.

Expected:
- Dramatic reaction.
- Radiation and thermal effects visible.
- No crash.
- No freeze.
- No unlimited particle creation.

## Acceptance Criteria

Stage 3 is complete when:
- `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR` are registered and compile.
- `FISS` fission emits Stage 3 effects in the required order.
- Thermal flash heats exposed targets before pressure effects.
- `FPRD` has low-level Stage 3 aftermath behavior.
- All Stage 3 particle creation uses per-frame caps.
- `HEXP`, `BEXP`, `SEXP`, `SHOK`, and `PDRV` do not create radiation products by themselves.
- `NTRN` remains the main fission-chain driver.
- `FISS` remains the main high-energy source.
- Full compile succeeds.
- Source checks find no added real-world nuclear/weapon modelling terms.
- Manual tests pass or any known tuning limitations are documented.

## Reporting Requirements

After implementation, report:
- files changed,
- elements added,
- elements modified,
- constants added,
- how `FISS` fission changed,
- how `FPRD` changed,
- how thermal flash works,
- how `HRAY` differs from `XRAY`,
- how to tune `HRAY`, `XRAY`, `RADS`, and `IONZ`,
- how to verify `RMTR`,
- manual tests run and results,
- known limitations.
