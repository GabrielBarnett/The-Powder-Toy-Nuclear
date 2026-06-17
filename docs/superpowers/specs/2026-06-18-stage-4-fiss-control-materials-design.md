# Stage 4 FISS Control Materials And Documentation Design Spec

## Source And Status

This spec is derived from `stage_4_design.txt` and assumes Stages 1 through 3 are present on `master`.

Current system status:
- `FISS` stores fictional compression in `tmp`.
- `FISS` stores fictional activation in `tmp2`.
- `NTRN` remains the main fictional chain-driver particle.
- `FISS` fission creates heat, pressure, `NTRN`, optional `SHOK`, `FPRD`, `HRAY`, `XRAY`, `RADS`, `IONZ`, and thermal flash effects.
- `CMTR` is the compression/activation diagnostic.
- `RMTR` is the radiation/energy diagnostic for Stage 3 effects.
- Stage 2 pressure tools and materials exist: `HEXP`, `BEXP`, `SEXP`, `SHOK`, `DENS`, `PABS`, `PWAL`, and `PDRV`.

Stage 4 must preserve the existing fictional compression-driven `FISS` design. It adds player-facing control materials, retunes `HRAY` and `RADS`, and documents the mod.

## Safety And Scope

This is a fictional particle sandbox mechanic only. All values must be arbitrary gameplay constants.

Do not add or model:
- real-world nuclear weapon parameters,
- real isotope data,
- real moderator or absorber materials,
- real shielding tables,
- real neutron cross-sections,
- real criticality concepts,
- real reflectors, tampers, staged devices, or weapon geometry,
- real explosive lensing,
- fusion fuels,
- yield calculations,
- real-world nuclear engineering formulas.

Stage 4 control materials are abstract gameplay tools. They must not encode real neutron control, radiation shielding, pressure mitigation, or weapon-component behavior. They should be named, tuned, and documented as fictional sandbox controls.

## Design Goals

1. Retune `HRAY` so it uses the safest supported maximum element temperature and lasts longer than Stage 3.
2. Retune `RADS` so new particles get random lifetimes ranging from short-lived to several minutes of simulation time.
3. Add fictional neutron control elements: `NABS`, `NSLW`, and `NDIFF`.
4. Add fictional radiation/radiant-heat control elements: `RABS` and `RWAL`.
5. Add fictional thermal and pressure management elements: `HSINK` and `PDMP`.
6. Integrate the controls into `NTRN`, `HRAY`, `XRAY`, `RADS`, thermal flash, `RMTR`, and README documentation.
7. Preserve all Stage 3 caps and add any new caps needed to prevent unlimited particles, heat, pressure, or update cost.

## Recommended Approach

Implement Stage 4 as a bounded control-material pass:

1. Add Stage 4 constants to the existing `FISS.h` tuning area.
2. Retune `HRAY` life and start temperature.
3. Replace fixed `RADS` life assignment with random `RADS_LIFE_MIN` / `RADS_LIFE_MAX` assignment.
4. Add `NABS`, `NSLW`, and `NDIFF`, then integrate them into `NTRN`.
5. Add `RABS` and `RWAL`, then integrate them into `HRAY`, `XRAY`, and `ApplyThermalFlash`.
6. Add `HSINK` and `PDMP` as simple bounded update-only control materials.
7. Expand `RMTR` to include neutron intensity without overloading `CMTR`.
8. Update README with all staged elements, tuning groups, caps, manual tests, and safety/scope limits.
9. Compile after each integration slice.
10. Run source-level checks and manual sandbox tests.

This approach keeps the highest-risk integrations small: first retune existing Stage 3 behavior, then add neutron controls, then radiant-energy controls, then passive heat/pressure controls.

## Tuning Organization

Keep Stage 4 tuning constants in `src/simulation/elements/FISS.h` with the existing fictional system constants. Group them plainly:

- `HRAY` Stage 4 life and temperature constants,
- `RADS` Stage 4 lifetime constants,
- `NABS` constants,
- `NSLW` constants,
- `NDIFF` constants,
- `RABS` constants,
- `RWAL` constants,
- `HSINK` constants,
- `PDMP` constants,
- new per-frame caps or interaction caps if required.

All constants must be gameplay values only. Do not use real material names, real physical units, or real engineering values.

## Required Elements

### NABS

`NABS` is a fictional neutron absorber.

Purpose:
- Gives players a way to reduce `NTRN` activity.
- Helps stop runaway fictional `FISS` reactions.
- Absorbs or dampens `NTRN` without reflecting, focusing, multiplying, or amplifying it.

Required behavior:
- Solid and inert.
- When `NTRN` touches or passes nearby, `NABS` may absorb the `NTRN`.
- Absorption produces modest heat and temporary damage/activation stored on `NABS`.
- `NABS` must not emit `NTRN`.
- `NABS` must not increase `FISS.tmp`, `FISS.tmp2`, or fission chance.
- Heavy use degrades `NABS` into a spent inert material such as `BRCK` or `STNE`.

Required constants:
- `NABS_ABSORPTION_CHANCE`
- `NABS_HEAT_ON_ABSORB`
- `NABS_DAMAGE_PER_ABSORB`
- `NABS_SPENT_THRESHOLD`
- `NABS_SPENT_ELEMENT`

### NSLW

`NSLW` is a fictional neutron slower / conditioner.

Purpose:
- Makes `NTRN` movement easier to observe and tune.
- Provides gamey neutron control without real moderation behavior.

Required behavior:
- Solid or porous-looking inert material.
- When `NTRN` passes through or near `NSLW`, it may reduce `NTRN` velocity.
- It applies `NSLW_LIFE_CHANGE` to `NTRN.life`; the constant may be zero if tuning shows life reduction is too aggressive.
- It may add modest heat to `NSLW`.
- It must not create extra `NTRN`.
- It must not reflect, focus, amplify, or make `FISS` more reactive.

Required constants:
- `NSLW_SLOW_CHANCE`
- `NSLW_SPEED_MULTIPLIER`
- `NSLW_LIFE_CHANGE`
- `NSLW_HEAT_ON_INTERACTION`

### NDIFF

`NDIFF` is a fictional neutron diffuser / scatterer.

Purpose:
- Randomizes `NTRN` direction.
- Spreads or disrupts `NTRN` paths in an arbitrary gameplay way.
- Weakens narrow `NTRN` streams without becoming a reflector.

Required behavior:
- Solid and inert.
- When `NTRN` interacts with `NDIFF`, randomize the `NTRN` velocity direction.
- Apply `NDIFF_SPEED_MULTIPLIER` and `NDIFF_LIFE_PENALTY`; either value may be tuned to a neutral effect if needed.
- May absorb `NTRN` with a low arbitrary chance.
- Must not increase `NTRN` count.
- Must not focus `NTRN` into more reactive paths.

Required constants:
- `NDIFF_SCATTER_CHANCE`
- `NDIFF_SPEED_MULTIPLIER`
- `NDIFF_LIFE_PENALTY`
- `NDIFF_ABSORPTION_CHANCE`

### RABS

`RABS` is a fictional radiation absorber for Stage 3 energy particles.

Purpose:
- Gives players a way to block or dampen `XRAY` and `HRAY`.
- Converts absorbed energy into heat.
- Does not manage `NTRN`; neutron control belongs to `NABS`, `NSLW`, and `NDIFF`.

Required behavior:
- Solid and inert.
- Absorbs `XRAY` and `HRAY` with separate tunable chances.
- Adds heat to itself on absorption.
- May create a small `IONZ` effect when overloaded.
- Can fail through ordinary temperature behavior if it becomes too hot.
- Must not reflect or amplify radiation-like particles.

Required constants:
- `RABS_XRAY_ABSORPTION_CHANCE`
- `RABS_HRAY_ABSORPTION_CHANCE`
- `RABS_HEAT_ON_XRAY_ABSORB`
- `RABS_HEAT_ON_HRAY_ABSORB`
- `RABS_OVERLOAD_TEMP`
- `RABS_IONZ_ON_OVERLOAD_CHANCE`

### RWAL

`RWAL` is a fictional radiation / thermal wall.

Purpose:
- Blocks or strongly reduces Stage 3 thermal flash line-of-sight.
- Strongly absorbs `HRAY` and `XRAY`.
- Supports sandbox testing of radiation and radiant-heat effects.

Required behavior:
- Solid wall-like material.
- `ApplyThermalFlash` treats `RWAL` as a blocker.
- `HRAY` and `XRAY` are absorbed more strongly by `RWAL` than by ordinary solids.
- Absorption heats `RWAL`.
- `RWAL` may fail or melt through normal temperature logic if overheated.
- Must not reflect radiation-like particles.
- Must not affect `NTRN` unless a later approved design explicitly adds that behavior.

Required constants:
- `RWAL_THERMAL_FLASH_BLOCKING`
- `RWAL_XRAY_ABSORPTION_CHANCE`
- `RWAL_HRAY_ABSORPTION_CHANCE`
- `RWAL_HEAT_ON_ABSORB`
- `RWAL_FAILURE_TEMP`

### HSINK

`HSINK` is a fictional thermal sink / heat dump.

Purpose:
- Absorbs local heat slowly.
- Helps stabilize test builds and cool hot `FPRD` / `RADS` areas.
- Does not interact with neutrons or radiation-like particles.

Required behavior:
- Solid and inert.
- Samples nearby particles in a small radius.
- Pulls a bounded amount of heat from hotter nearby particles.
- Stores absorbed heat in its own temperature.
- Can overheat or fail through ordinary temperature behavior.
- Must not emit radiation, pressure, `NTRN`, `HRAY`, `XRAY`, `RADS`, or `IONZ`.
- Must not make `FISS` more reactive.

Required constants:
- `HSINK_HEAT_ABSORPTION_RATE`
- `HSINK_SAMPLE_RADIUS`
- `HSINK_MAX_SAFE_TEMP`
- `HSINK_FAILURE_TEMP`

### PDMP

`PDMP` is a fictional pressure damper.

Purpose:
- Reduces local pressure spikes.
- Helps stabilize pressure-tool and `FISS` test builds.
- Does not create pressure or increase compression.

Required behavior:
- Solid and inert.
- Samples nearby pressure cells.
- Reduces positive pressure by a bounded amount.
- May heat up or accumulate damage from repeated pressure exposure.
- May degrade into inert debris after enough damage.
- Must not create pressure.
- Must not directly increase `FISS.tmp`.
- Must not behave like a weapon component.

Required constants:
- `PDMP_PRESSURE_DAMPING_RATE`
- `PDMP_PRESSURE_SAMPLE_RADIUS`
- `PDMP_DAMAGE_PER_PRESSURE`
- `PDMP_FAILURE_THRESHOLD`

## HRAY Retune

`HRAY` must be retuned for Stage 4:

- Increase `HRAY_LIFE_MIN` and `HRAY_LIFE_MAX` so `HRAY` lasts longer than Stage 3.
- Set the default/spawn temperature to the safest supported maximum temperature used by the game engine, clamped through existing temperature helpers.
- Keep `MAX_HRAY_CREATED_PER_FRAME`.
- Do not increase `HRAY` particle count simply because life is longer.
- Preserve the distinction between `HRAY` and `XRAY`: `HRAY` is radiant heat, `XRAY` is penetrating energy and ionisation visuals.

Suggested new constants:
- `HRAY_STAGE4_TEMP`
- updated `HRAY_LIFE_MIN`
- updated `HRAY_LIFE_MAX`

## RADS Retune

`RADS` must receive random lifetimes:

- Replace fixed `RADS_START_LIFE` assignment with `RADS_LIFE_MIN` / `RADS_LIFE_MAX`.
- `RADS_LIFE_MIN` should allow short-lived debris.
- `RADS_LIFE_MAX` should allow some particles to persist for several minutes of simulation time.
- All `RADS` emissions must remain capped by `MAX_RADS_EMISSIONS_PER_FRAME`.
- `RADS` must eventually decay into inert residue.

Required constants:
- `RADS_LIFE_MIN`
- `RADS_LIFE_MAX`

`RADS_START_LIFE` may be kept as a compatibility alias only if useful; otherwise replace it with the min/max model.

## Integration Requirements

### NTRN

Update `NTRN` so it checks nearby or contacted Stage 4 neutron controls:

- `NABS` can absorb and remove `NTRN`.
- `NSLW` can slow `NTRN` and apply the configured life change.
- `NDIFF` can scatter, weaken, or rarely absorb `NTRN`.
- These interactions must not create additional `NTRN`.
- These interactions must not focus, reflect, or amplify neutron behavior.
- Bounds checks and particle validity checks are required.

The simplest acceptable implementation is a small neighbor scan in `NTRN.cpp` using helper functions or clearly grouped local logic.

### HRAY And XRAY

Update `HRAY` and `XRAY` so they check `RABS` and `RWAL`:

- `RABS` can absorb `HRAY` and `XRAY`, adding heat to itself.
- `RWAL` can absorb `HRAY` and `XRAY` with stronger chances.
- Overloaded `RABS` may create small `IONZ` effects through existing capped helpers.
- Absorption kills the incoming `HRAY` or `XRAY`.
- No reflection, particle multiplication, or recursive radiation loops.

### Thermal Flash

Update `ApplyThermalFlash`:

- `RWAL` must block or strongly reduce line-of-sight thermal flash.
- Ordinary solids should continue to block thermal flash as in Stage 3.
- `RABS` blocks thermal flash through the ordinary solid line-of-sight behavior; direct `RABS` flash-specific absorption is out of scope for Stage 4.
- Thermal flash must remain capped by `MAX_THERMAL_FLASH_EVENTS_PER_FRAME`.

### RMTR

`RMTR` should continue detecting:
- `HRAY`
- `XRAY`
- `RADS`
- `IONZ`

Add neutron intensity support:
- `RMTR` must expose a nearby `NTRN` intensity mode, and
- the combined diagnostic value should include nearby `NTRN` activity.

`RMTR` must not emit or alter particles.

### CMTR

Keep `CMTR` focused on `FISS` compression, activation, and pressure readiness. Do not overload `CMTR` with Stage 4 radiation controls if `RMTR` can report that activity.

## README Requirements

Update `README.md` with a clearly marked mod section. It must document:

- Stage 1 through Stage 4 feature summary.
- `FISS`, `NTRN`, `FPRD`, `CMTR`, `HEXP`, `BEXP`, `SEXP`, `SHOK`, `DENS`, `PABS`, `PWAL`, `PDRV`, `HRAY`, `XRAY`, `RADS`, `IONZ`, `RMTR`.
- Stage 4 elements: `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, `PDMP`.
- `HRAY` max-temperature / longer-life retune.
- `RADS` random long-lifetime retune.
- Major tuning constant groups.
- Per-frame caps and why they exist.
- Manual Stage 4 tests.
- A plain statement that all mechanics are fictional gameplay controls and not real nuclear engineering, real shielding, real neutron physics, or weapon-design guidance.

The README update should be concise enough to be useful to a player or future tuner without turning the upstream README into a full design document.

## Registration And Files

Add new element files:
- `src/simulation/elements/NABS.cpp`
- `src/simulation/elements/NSLW.cpp`
- `src/simulation/elements/NDIFF.cpp`
- `src/simulation/elements/RABS.cpp`
- `src/simulation/elements/RWAL.cpp`
- `src/simulation/elements/HSINK.cpp`
- `src/simulation/elements/PDMP.cpp`

Modify:
- `src/simulation/elements/FISS.h`
- `src/simulation/elements/FISS.cpp`
- `src/simulation/elements/NTRN.cpp`
- `src/simulation/elements/HRAY.cpp`
- `src/simulation/elements/XRAY.cpp`
- `src/simulation/elements/RADS.cpp`
- `src/simulation/elements/RMTR.cpp`
- `src/simulation/elements/meson.build`
- `README.md`

Register all new element names in `src/simulation/elements/meson.build`.

## Stability And Caps

Stage 4 must preserve:
- `MAX_FISSION_EVENTS_PER_FRAME`
- `MAX_NTRN_CREATED_PER_FRAME`
- `MAX_SHOK_CREATED_PER_FRAME`
- `MAX_HRAY_CREATED_PER_FRAME`
- `MAX_XRAY_CREATED_PER_FRAME`
- `MAX_RADS_CREATED_PER_FRAME`
- `MAX_IONZ_CREATED_PER_FRAME`
- `MAX_RADS_EMISSIONS_PER_FRAME`
- `MAX_THERMAL_FLASH_EVENTS_PER_FRAME`

Add new caps only if required by implementation cost:
- `MAX_STAGE4_CONTROL_INTERACTIONS_PER_FRAME`
- `MAX_RABS_IONZ_CREATED_PER_FRAME`

Required safeguards:
- No Stage 4 control material may create `FISS`.
- No Stage 4 control material may create extra `NTRN`.
- No Stage 4 control material may increase `FISS.tmp` or `FISS.tmp2`.
- No control material may reflect, focus, or amplify radiation-like particles.
- No material may add unbounded heat or pressure.
- All neighbor scans must be small and bounds-safe.
- All particle creation must handle failure safely.
- Moderate mixed setups must not crash, freeze, or create unlimited particles.

## Manual Tests

### Test 1: NABS Neutron Absorption

Fire `NTRN` at `NABS`.

Expected:
- `NTRN` is absorbed or removed with tunable probability.
- `NABS` heats modestly.
- No extra `NTRN` is created.

### Test 2: NSLW Neutron Slowing

Fire `NTRN` through or near `NSLW`.

Expected:
- `NTRN` velocity is reduced.
- `NTRN` remains finite-lived.
- No focusing or particle multiplication occurs.

### Test 3: NDIFF Neutron Scattering

Fire `NTRN` through or near `NDIFF`.

Expected:
- `NTRN` direction is randomized or weakened.
- No extra `NTRN` is created.

### Test 4: RABS Radiation Absorption

Fire `XRAY` and `HRAY` at `RABS`.

Expected:
- Energy particles are absorbed with tunable probability.
- `RABS` heats.
- No reflection or amplification occurs.

### Test 5: RWAL Thermal Flash Blocking

Place `RWAL` between a `FISS` reaction and a heat-sensitive target.

Expected:
- Target receives much less `HRAY` / thermal flash heating.
- `RWAL` heats when absorbing energy.

### Test 6: HSINK Heat Control

Place `HSINK` near hot `FPRD` or `RADS`.

Expected:
- Nearby heat is reduced over time.
- `HSINK` warms up.
- No radiation or pressure is emitted.

### Test 7: PDMP Pressure Damping

Place `PDMP` near `HEXP`, `BEXP`, `SEXP`, or `SHOK`.

Expected:
- Local pressure spikes are reduced.
- `PDMP` may heat or degrade.
- `PDMP` does not create pressure or increase `FISS` compression.

### Test 8: RADS Long Life

Spawn many `RADS` particles.

Expected:
- Lifetimes vary.
- Some `RADS` decay quickly.
- Some persist for several minutes.
- All eventually decay into inert residue.

### Test 9: RMTR Neutron/Radiation Diagnostics

Place `RMTR` near `NTRN`, `HRAY`, `XRAY`, `RADS`, and `IONZ`.

Expected:
- Existing radiation readings still work.
- New `NTRN` indication works if implemented.
- `RMTR` does not alter particles.

### Test 10: Stability

Use `FISS`, `NTRN`, `HRAY`, `XRAY`, `RADS`, `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, and `PDMP` in a moderate setup.

Expected:
- No crash.
- No freeze.
- No infinite particles.
- No unbounded heat or pressure.
- `FISS` remains the only compact high-energy reaction source.

## Acceptance Criteria

Stage 4 is acceptable when:
- `HRAY` lasts longer and starts at the safest supported maximum temperature.
- `RADS` uses random lifetimes with a long upper range.
- `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, and `PDMP` are registered, compile, and appear as fictional control elements.
- `NTRN` integrates with `NABS`, `NSLW`, and `NDIFF`.
- `HRAY` and `XRAY` integrate with `RABS` and `RWAL`.
- `RWAL` blocks or strongly reduces thermal flash line-of-sight.
- `HSINK` reduces local heat without emitting particles.
- `PDMP` reduces local pressure without creating pressure.
- `RMTR` still reports Stage 3 radiation products and includes `NTRN`.
- README documents all stages, Stage 4 materials, tuning constants, caps, safety limits, and manual tests.
- Full compile succeeds.
- Source checks find no implementation of real-world nuclear, shielding, pressure, or weapon-design models.
- Manual tests pass or any deferred manual testing is explicitly reported.

## Reporting Requirements

After implementation, report:
- files changed,
- elements added,
- elements modified,
- constants added or retuned,
- how `HRAY` changed,
- how `RADS` lifetime changed,
- how neutron controls work,
- how radiant-energy controls work,
- how heat and pressure controls work,
- how README changed,
- verification commands and results,
- manual tests run and results,
- known limitations or follow-up tuning.
