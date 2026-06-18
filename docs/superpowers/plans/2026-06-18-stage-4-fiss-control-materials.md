# Stage 4 FISS Control Materials Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add Stage 4 fictional neutron, radiant-energy, heat, and pressure control materials; retune `HRAY` and `RADS`; update README documentation.

**Architecture:** Keep Stage 4 inside the existing `FissStage1` tuning/helper surface and existing element-per-file pattern. Register seven new focused element files, integrate their interactions into `NTRN`, `HRAY`, `XRAY`, `RADS`, `RMTR`, and `FISS.cpp` thermal flash, then update README with scoped gameplay documentation. Preserve Stage 1-3 behavior and caps.

**Tech Stack:** C++20, The Powder Toy element API, Meson/Ninja, PowerShell verification commands on Windows.

---

## File Structure

- Modify `src/simulation/elements/FISS.h`: add Stage 4 constants and helper declarations.
- Modify `src/simulation/elements/FISS.cpp`: add shared helper functions for Stage 4 control interactions and thermal-flash blocking.
- Modify `src/simulation/elements/NTRN.cpp`: add `NABS`, `NSLW`, and `NDIFF` interaction checks.
- Modify `src/simulation/elements/HRAY.cpp`: retune temperature/life and add `RABS` / `RWAL` absorption.
- Modify `src/simulation/elements/XRAY.cpp`: add `RABS` / `RWAL` absorption.
- Modify `src/simulation/elements/RADS.cpp`: use random min/max lifetime.
- Modify `src/simulation/elements/RMTR.cpp`: add `NTRN` diagnostic mode and combined signal contribution.
- Modify `src/simulation/elements/meson.build`: register `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, and `PDMP`.
- Modify `README.md`: add a concise mod section describing Stages 1-4, fictional scope, tuning groups, caps, and manual tests.
- Create `src/simulation/elements/NABS.cpp`: fictional neutron absorber.
- Create `src/simulation/elements/NSLW.cpp`: fictional neutron slower.
- Create `src/simulation/elements/NDIFF.cpp`: fictional neutron diffuser.
- Create `src/simulation/elements/RABS.cpp`: fictional radiant-energy absorber.
- Create `src/simulation/elements/RWAL.cpp`: fictional radiation/thermal wall.
- Create `src/simulation/elements/HSINK.cpp`: fictional heat sink.
- Create `src/simulation/elements/PDMP.cpp`: fictional pressure damper.

Use this compile command after each implementation checkpoint:

```powershell
$env:PATH="C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts;C:\Users\Gabriel\.cache\codex-build-tools\msys64\ucrt64\bin;C:\Users\Gabriel\.cache\codex-build-tools\w64devkit\bin;$env:PATH"
& "C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts\meson.exe" compile -C build
```

Expected: exit code `0`. If the linker reports `cannot open output file powder.exe: Permission denied`, close the running `build\powder.exe` process and rerun the same compile command.

## Task 1: Add Stage 4 Constants And Declarations

**Files:**
- Modify: `src/simulation/elements/FISS.h`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
foreach ($name in @(
  "HRAY_STAGE4_TEMP",
  "RADS_LIFE_MIN",
  "RADS_LIFE_MAX",
  "NABS_ABSORPTION_CHANCE",
  "NSLW_SLOW_CHANCE",
  "NDIFF_SCATTER_CHANCE",
  "RABS_XRAY_ABSORPTION_CHANCE",
  "RWAL_THERMAL_FLASH_BLOCKING",
  "HSINK_HEAT_ABSORPTION_RATE",
  "PDMP_PRESSURE_DAMPING_RATE",
  "TryAbsorbRadiantParticle",
  "IsThermalFlashBlocker"
)) {
  if ($header -notmatch $name) { throw "RED: $name missing" }
}
```

Expected before implementation: fails with `RED: HRAY_STAGE4_TEMP missing`.

- [ ] **Step 2: Add constants**

In `src/simulation/elements/FISS.h`, replace the existing `HRAY` life constants:

```cpp
	constexpr int HRAY_LIFE_MIN = 5;
	constexpr int HRAY_LIFE_MAX = 11;
```

with:

```cpp
	constexpr float HRAY_STAGE4_TEMP = MAX_TEMP;
	constexpr int HRAY_LIFE_MIN = 14;
	constexpr int HRAY_LIFE_MAX = 28;
```

Then replace:

```cpp
	constexpr int RADS_START_LIFE = 520;
```

with:

```cpp
	constexpr int RADS_LIFE_MIN = 240;
	constexpr int RADS_LIFE_MAX = 10800;
	constexpr int RADS_START_LIFE = RADS_LIFE_MIN;
```

Insert this Stage 4 block after the existing `FPRD_TO_ASH_TIME` constant:

```cpp
	constexpr int NABS_ABSORPTION_CHANCE = 7600;
	constexpr float NABS_HEAT_ON_ABSORB = 85.0f;
	constexpr int NABS_DAMAGE_PER_ABSORB = 8;
	constexpr int NABS_SPENT_THRESHOLD = 240;
	constexpr int NABS_SPENT_ELEMENT = PT_BRCK;

	constexpr int NSLW_SLOW_CHANCE = 7000;
	constexpr float NSLW_SPEED_MULTIPLIER = 0.45f;
	constexpr int NSLW_LIFE_CHANGE = -8;
	constexpr float NSLW_HEAT_ON_INTERACTION = 18.0f;

	constexpr int NDIFF_SCATTER_CHANCE = 6800;
	constexpr float NDIFF_SPEED_MULTIPLIER = 0.78f;
	constexpr int NDIFF_LIFE_PENALTY = 5;
	constexpr int NDIFF_ABSORPTION_CHANCE = 850;

	constexpr int RABS_XRAY_ABSORPTION_CHANCE = 7800;
	constexpr int RABS_HRAY_ABSORPTION_CHANCE = 8200;
	constexpr float RABS_HEAT_ON_XRAY_ABSORB = 140.0f;
	constexpr float RABS_HEAT_ON_HRAY_ABSORB = 320.0f;
	constexpr float RABS_OVERLOAD_TEMP = 1450.0f + 273.15f;
	constexpr int RABS_IONZ_ON_OVERLOAD_CHANCE = 1200;

	constexpr bool RWAL_THERMAL_FLASH_BLOCKING = true;
	constexpr int RWAL_XRAY_ABSORPTION_CHANCE = 9000;
	constexpr int RWAL_HRAY_ABSORPTION_CHANCE = 9400;
	constexpr float RWAL_HEAT_ON_ABSORB = 230.0f;
	constexpr float RWAL_FAILURE_TEMP = 2400.0f + 273.15f;

	constexpr float HSINK_HEAT_ABSORPTION_RATE = 16.0f;
	constexpr int HSINK_SAMPLE_RADIUS = 2;
	constexpr float HSINK_MAX_SAFE_TEMP = 1800.0f + 273.15f;
	constexpr float HSINK_FAILURE_TEMP = 2600.0f + 273.15f;

	constexpr float PDMP_PRESSURE_DAMPING_RATE = 0.72f;
	constexpr int PDMP_PRESSURE_SAMPLE_RADIUS = 1;
	constexpr int PDMP_DAMAGE_PER_PRESSURE = 2;
	constexpr int PDMP_FAILURE_THRESHOLD = 220;
```

- [ ] **Step 3: Update declarations**

In `src/simulation/elements/FISS.h`, after `TryUseRadsEmissionSlot`, add:

```cpp
	bool TryAbsorbRadiantParticle(Simulation *sim, int particleIndex, int absorberIndex, int absorberType, bool heatRay);
	bool IsThermalFlashBlocker(int type);
	int RandomRadsLife(Simulation *sim);
```

- [ ] **Step 4: Run the source check again**

Run the command from Step 1.

Expected: exits successfully with no output.

- [ ] **Step 5: Compile**

Run the compile command from the File Structure section.

Expected: exit code `0`.

## Task 2: Register And Add Stage 4 Control Element Files

**Files:**
- Modify: `src/simulation/elements/meson.build`
- Create: `src/simulation/elements/NABS.cpp`
- Create: `src/simulation/elements/NSLW.cpp`
- Create: `src/simulation/elements/NDIFF.cpp`
- Create: `src/simulation/elements/RABS.cpp`
- Create: `src/simulation/elements/RWAL.cpp`
- Create: `src/simulation/elements/HSINK.cpp`
- Create: `src/simulation/elements/PDMP.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$meson = Get-Content -Raw "src\simulation\elements\meson.build"
foreach ($name in @("'NABS'", "'NSLW'", "'NDIFF'", "'RABS'", "'RWAL'", "'HSINK'", "'PDMP'")) {
  if ($meson -notmatch [regex]::Escape($name)) { throw "RED: $name not registered" }
}
foreach ($file in @("NABS.cpp", "NSLW.cpp", "NDIFF.cpp", "RABS.cpp", "RWAL.cpp", "HSINK.cpp", "PDMP.cpp")) {
  if (!(Test-Path "src\simulation\elements\$file")) { throw "RED: $file missing" }
}
```

Expected before implementation: fails with `RED: 'NABS' not registered`.

- [ ] **Step 2: Register elements**

In `src/simulation/elements/meson.build`, append after `'RMTR',`:

```meson
	'NABS',
	'NSLW',
	'NDIFF',
	'RABS',
	'RWAL',
	'HSINK',
	'PDMP',
```

- [ ] **Step 3: Create simple inert element files**

Create `src/simulation/elements/NABS.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_NABS()
{
	Identifier = "DEFAULT_PT_NABS";
	Name = "NABS";
	Colour = 0x4A5F68_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 42;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 55;
	Description = "Fictional neutron absorber. Removes NTRN without reflection or amplification.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].tmp >= FissStage1::NABS_SPENT_THRESHOLD)
	{
		sim->part_change_type(i, x, y, FissStage1::NABS_SPENT_ELEMENT);
		parts[i].temp = restrict_flt(parts[i].temp, MIN_TEMP, MAX_TEMP);
		return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int damage = std::clamp(cpart->tmp, 0, FissStage1::NABS_SPENT_THRESHOLD);
	float glow = float(damage) / float(FissStage1::NABS_SPENT_THRESHOLD);
	*colr = std::clamp(*colr + int(80.0f * glow), 0, 255);
	*colg = std::clamp(*colg + int(60.0f * glow), 0, 255);
	return 0;
}
```

Create `src/simulation/elements/NSLW.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

void Element::Element_NSLW()
{
	Identifier = "DEFAULT_PT_NSLW";
	Name = "NSLW";
	Colour = 0x546C57_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 32;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 45;
	Description = "Fictional neutron slower. Dampens NTRN velocity for sandbox tuning.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
}
```

Create `src/simulation/elements/NDIFF.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

void Element::Element_NDIFF()
{
	Identifier = "DEFAULT_PT_NDIFF";
	Name = "NDIF";
	Colour = 0x6D646F_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 34;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 45;
	Description = "Fictional neutron diffuser. Randomises NTRN direction without multiplying it.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
}
```

Create `src/simulation/elements/RABS.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_RABS()
{
	Identifier = "DEFAULT_PT_RABS";
	Name = "RABS";
	Colour = 0x536076_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 48;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 65;
	Description = "Fictional radiant-energy absorber for HRAY and XRAY.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::RABS_OVERLOAD_TEMP;
	HighTemperatureTransition = PT_LAVA;
	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	parts[i].tmp = std::max(parts[i].tmp - 1, 0);
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int glow = std::clamp(cpart->tmp, 0, 80);
	if (glow > 0)
	{
		*firea = glow;
		*firer = 90;
		*fireg = 145;
		*fireb = 255;
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
```

Create `src/simulation/elements/RWAL.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "simulation/Air.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_RWAL()
{
	Identifier = "DEFAULT_PT_RWAL";
	Name = "RWAL";
	Colour = 0x39404A_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 70;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 70;
	Description = "Fictional radiation and thermal wall. Blocks flash and absorbs HRAY/XRAY.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::RWAL_FAILURE_TEMP;
	HighTemperatureTransition = PT_LAVA;
	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->air->bmap_blockair[y / CELL][x / CELL] = 1;
	sim->air->bmap_blockairh[y / CELL][x / CELL] = 0x8;
	return 0;
}
```

Create `src/simulation/elements/HSINK.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HSINK()
{
	Identifier = "DEFAULT_PT_HSINK";
	Name = "HSNK";
	Colour = 0x31535A_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 40;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 95;
	Description = "Fictional heat sink. Pulls bounded heat from nearby particles.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::HSINK_FAILURE_TEMP;
	HighTemperatureTransition = PT_LAVA;
	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	for (int dy = -FissStage1::HSINK_SAMPLE_RADIUS; dy <= FissStage1::HSINK_SAMPLE_RADIUS; dy++)
	{
		for (int dx = -FissStage1::HSINK_SAMPLE_RADIUS; dx <= FissStage1::HSINK_SAMPLE_RADIUS; dx++)
		{
			if (!dx && !dy)
				continue;
			if (dx * dx + dy * dy > FissStage1::HSINK_SAMPLE_RADIUS * FissStage1::HSINK_SAMPLE_RADIUS)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (!r || parts[ID(r)].temp <= parts[i].temp)
				continue;
			float pulled = std::min((parts[ID(r)].temp - parts[i].temp) * 0.08f, FissStage1::HSINK_HEAT_ABSORPTION_RATE);
			parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp - pulled, MIN_TEMP, MAX_TEMP);
			parts[i].temp = restrict_flt(parts[i].temp + pulled * 0.55f, MIN_TEMP, MAX_TEMP);
		}
	}
	if (parts[i].temp > FissStage1::HSINK_MAX_SAFE_TEMP)
		parts[i].tmp++;
	return 0;
}
```

Create `src/simulation/elements/PDMP.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_PDMP()
{
	Identifier = "DEFAULT_PT_PDMP";
	Name = "PDMP";
	Colour = 0x47505C_rgb;
	MenuVisible = 1;
	MenuSection = SC_FORCE;
	Enabled = 1;
	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;
	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 38;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 45;
	Description = "Fictional pressure damper. Reduces pressure spikes without creating pressure.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	for (int dy = -FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dy <= FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dy++)
	{
		for (int dx = -FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dx <= FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dx++)
		{
			int cx = x / CELL + dx;
			int cy = y / CELL + dy;
			if (!InCellBounds(cx, cy))
				continue;
			float pressure = sim->pv[cy][cx];
			if (pressure <= 0.0f)
				continue;
			sim->pv[cy][cx] = restrict_flt(pressure * FissStage1::PDMP_PRESSURE_DAMPING_RATE, MIN_PRESSURE, MAX_PRESSURE);
			if (pressure > 2.0f)
				parts[i].tmp += FissStage1::PDMP_DAMAGE_PER_PRESSURE;
		}
	}
	if (parts[i].tmp >= FissStage1::PDMP_FAILURE_THRESHOLD)
	{
		sim->part_change_type(i, x, y, PT_BRCK);
		return 1;
	}
	return 0;
}
```

- [ ] **Step 4: Run source check and compile**

Run the source check from Step 1.

Expected: prints no error.

Run the compile command from the File Structure section.

Expected: exit code `0`.

## Task 3: Retune HRAY And RADS Lifetime Behavior

**Files:**
- Modify: `src/simulation/elements/HRAY.cpp`
- Modify: `src/simulation/elements/RADS.cpp`
- Modify: `src/simulation/elements/FISS.cpp`
- Modify: `src/simulation/elements/FPRD.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$hray = Get-Content -Raw "src\simulation\elements\HRAY.cpp"
$rads = Get-Content -Raw "src\simulation\elements\RADS.cpp"
$fiss = Get-Content -Raw "src\simulation\elements\FISS.cpp"
$fprd = Get-Content -Raw "src\simulation\elements\FPRD.cpp"
if ($hray -notmatch "HRAY_STAGE4_TEMP") { throw "RED: HRAY temp retune missing" }
if ($rads -notmatch "RandomRadsLife") { throw "RED: RADS create random life missing" }
if ($fiss -notmatch "RandomRadsLife") { throw "RED: TryCreateRADS random life missing" }
if ($fprd -notmatch "RandomRadsLife") { throw "RED: FPRD RADS conversion random life missing" }
```

Expected before implementation: fails with `RED: HRAY temp retune missing`.

- [ ] **Step 2: Update `FISS.cpp` helper**

In `src/simulation/elements/FISS.cpp`, inside `namespace FissStage1`, add this function before `TryCreateRADS`:

```cpp
	int RandomRadsLife(Simulation *sim)
	{
		return sim->rng.between(RADS_LIFE_MIN, RADS_LIFE_MAX);
	}
```

Then in `TryCreateRADS`, replace:

```cpp
			sim->parts[r].life = RADS_START_LIFE;
```

with:

```cpp
			sim->parts[r].life = RandomRadsLife(sim);
```

- [ ] **Step 3: Update `HRAY.cpp`**

In `Element_HRAY`, replace:

```cpp
	DefaultProperties.temp = FissStage1::IONZ_START_TEMP;
```

with:

```cpp
	DefaultProperties.temp = restrict_flt(FissStage1::HRAY_STAGE4_TEMP, MIN_TEMP, MAX_TEMP);
```

In `create`, after assigning `life`, add:

```cpp
	sim->parts[i].temp = restrict_flt(FissStage1::HRAY_STAGE4_TEMP, MIN_TEMP, MAX_TEMP);
```

- [ ] **Step 4: Update `RADS.cpp`**

In `create`, replace:

```cpp
	sim->parts[i].life = FissStage1::RADS_START_LIFE;
```

with:

```cpp
	sim->parts[i].life = FissStage1::RandomRadsLife(sim);
```

- [ ] **Step 5: Update `FPRD.cpp`**

In the `PT_RADS` conversion branch, replace:

```cpp
			parts[i].life = FissStage1::RADS_START_LIFE;
```

with:

```cpp
			parts[i].life = FissStage1::RandomRadsLife(sim);
```

- [ ] **Step 6: Run source check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command.

Expected: exit code `0`.

## Task 4: Integrate NABS, NSLW, And NDIFF With NTRN

**Files:**
- Modify: `src/simulation/elements/NTRN.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$ntrn = Get-Content -Raw "src\simulation\elements\NTRN.cpp"
foreach ($name in @("PT_NABS", "PT_NSLW", "PT_NDIFF", "NABS_ABSORPTION_CHANCE", "NSLW_SPEED_MULTIPLIER", "NDIFF_SCATTER_CHANCE")) {
  if ($ntrn -notmatch $name) { throw "RED: NTRN control integration missing $name" }
}
```

Expected before implementation: fails with `RED: NTRN control integration missing PT_NABS`.

- [ ] **Step 2: Add local NTRN control helpers**

In `src/simulation/elements/NTRN.cpp`, add these helper functions after the `static` declarations and before `Element_NTRN`:

```cpp
static void HeatControl(Parts &parts, int r, float amount)
{
	parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp + amount, MIN_TEMP, MAX_TEMP);
}

static void RandomizeVelocity(Simulation *sim, Particle &part, float multiplier)
{
	float speed = std::sqrt(part.vx * part.vx + part.vy * part.vy) * multiplier;
	if (speed < 0.05f)
		speed = 0.05f;
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	part.vx = cosf(angle) * speed;
	part.vy = sinf(angle) * speed;
}
```

- [ ] **Step 3: Add control interactions inside `update`**

Inside the existing neighbor scan in `update`, after reading `int r = pmap[ny][nx];`, insert this block before the `PT_FISS` check:

```cpp
			int type = TYP(r);
			if (type == PT_NABS)
			{
				if (sim->rng.chance(FissStage1::NABS_ABSORPTION_CHANCE, 10000))
				{
					HeatControl(parts, r, FissStage1::NABS_HEAT_ON_ABSORB);
					parts[ID(r)].tmp += FissStage1::NABS_DAMAGE_PER_ABSORB;
					sim->kill_part(i);
					return 1;
				}
				continue;
			}
			if (type == PT_NSLW)
			{
				if (sim->rng.chance(FissStage1::NSLW_SLOW_CHANCE, 10000))
				{
					parts[i].vx *= FissStage1::NSLW_SPEED_MULTIPLIER;
					parts[i].vy *= FissStage1::NSLW_SPEED_MULTIPLIER;
					parts[i].life = std::max(parts[i].life + FissStage1::NSLW_LIFE_CHANGE, 1);
					HeatControl(parts, r, FissStage1::NSLW_HEAT_ON_INTERACTION);
					interacted = true;
				}
				continue;
			}
			if (type == PT_NDIFF)
			{
				if (sim->rng.chance(FissStage1::NDIFF_ABSORPTION_CHANCE, 10000))
				{
					sim->kill_part(i);
					return 1;
				}
				if (sim->rng.chance(FissStage1::NDIFF_SCATTER_CHANCE, 10000))
				{
					RandomizeVelocity(sim, parts[i], FissStage1::NDIFF_SPEED_MULTIPLIER);
					parts[i].life = std::max(parts[i].life - FissStage1::NDIFF_LIFE_PENALTY, 1);
					interacted = true;
				}
				continue;
			}
```

Then replace:

```cpp
			if (TYP(r) != PT_FISS)
```

with:

```cpp
			if (type != PT_FISS)
```

- [ ] **Step 4: Run source check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command.

Expected: exit code `0`.

## Task 5: Integrate RABS And RWAL With HRAY, XRAY, And Thermal Flash

**Files:**
- Modify: `src/simulation/elements/FISS.cpp`
- Modify: `src/simulation/elements/HRAY.cpp`
- Modify: `src/simulation/elements/XRAY.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$fiss = Get-Content -Raw "src\simulation\elements\FISS.cpp"
$hray = Get-Content -Raw "src\simulation\elements\HRAY.cpp"
$xray = Get-Content -Raw "src\simulation\elements\XRAY.cpp"
foreach ($name in @("TryAbsorbRadiantParticle", "IsThermalFlashBlocker", "PT_RWAL")) {
  if ($fiss -notmatch $name) { throw "RED: FISS radiant helper missing $name" }
}
foreach ($name in @("TryAbsorbRadiantParticle", "PT_RABS", "PT_RWAL")) {
  if ($hray -notmatch $name) { throw "RED: HRAY absorption missing $name" }
  if ($xray -notmatch $name) { throw "RED: XRAY absorption missing $name" }
}
```

Expected before implementation: fails with `RED: FISS radiant helper missing TryAbsorbRadiantParticle`.

- [ ] **Step 2: Add helpers in `FISS.cpp`**

Inside `namespace FissStage1`, before `ApplyThermalFlash`, add:

```cpp
	bool TryAbsorbRadiantParticle(Simulation *sim, int particleIndex, int absorberIndex, int absorberType, bool heatRay)
	{
		if (absorberType != PT_RABS && absorberType != PT_RWAL)
			return false;
		int chance = 0;
		float heat = 0.0f;
		if (absorberType == PT_RABS)
		{
			chance = heatRay ? RABS_HRAY_ABSORPTION_CHANCE : RABS_XRAY_ABSORPTION_CHANCE;
			heat = heatRay ? RABS_HEAT_ON_HRAY_ABSORB : RABS_HEAT_ON_XRAY_ABSORB;
		}
		else
		{
			chance = heatRay ? RWAL_HRAY_ABSORPTION_CHANCE : RWAL_XRAY_ABSORPTION_CHANCE;
			heat = RWAL_HEAT_ON_ABSORB;
		}
		if (!Chance10000(sim, chance))
			return false;
		Particle &absorber = sim->parts[absorberIndex];
		absorber.temp = restrict_flt(absorber.temp + heat, MIN_TEMP, MAX_TEMP);
		absorber.tmp = std::min(absorber.tmp + 8, 255);
		if (absorberType == PT_RABS && absorber.temp >= RABS_OVERLOAD_TEMP && Chance10000(sim, RABS_IONZ_ON_OVERLOAD_CHANCE))
			TryCreateIONZ(sim, int(absorber.x + 0.5f), int(absorber.y + 0.5f), 1);
		sim->kill_part(particleIndex);
		return true;
	}

	bool IsThermalFlashBlocker(int type)
	{
		return RWAL_THERMAL_FLASH_BLOCKING && type == PT_RWAL;
	}
```

In `ApplyThermalFlash`, replace:

```cpp
					if (!IsRadiationTransparent(TYP(blocker)))
```

with:

```cpp
					if (IsThermalFlashBlocker(TYP(blocker)) || !IsRadiationTransparent(TYP(blocker)))
```

- [ ] **Step 3: Update `HRAY.cpp`**

Inside the solid-contact block, after `int type = TYP(r);` and before the gas/energy check, add:

```cpp
			if ((type == PT_RABS || type == PT_RWAL) && FissStage1::TryAbsorbRadiantParticle(sim, i, ID(r), type, true))
				return 1;
```

- [ ] **Step 4: Update `XRAY.cpp`**

Inside the solid-contact block, after `int type = TYP(r);` and before the `PT_FISS` check, add:

```cpp
			if ((type == PT_RABS || type == PT_RWAL) && FissStage1::TryAbsorbRadiantParticle(sim, i, ID(r), type, false))
				return 1;
```

- [ ] **Step 5: Run source check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command.

Expected: exit code `0`.

## Task 6: Expand RMTR NTRN Diagnostics

**Files:**
- Modify: `src/simulation/elements/RMTR.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$rmtr = Get-Content -Raw "src\simulation\elements\RMTR.cpp"
foreach ($name in @("PT_NTRN", "ntrn", "case 4")) {
  if ($rmtr -notmatch $name) { throw "RED: RMTR NTRN support missing $name" }
}
```

Expected before implementation: fails with `RED: RMTR NTRN support missing PT_NTRN`.

- [ ] **Step 2: Update RMTR sampling**

In `update`, after the `ionz` count line, add:

```cpp
	int ntrn = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_NTRN);
```

Change:

```cpp
	int mode = std::clamp(parts[i].ctype, 0, 5);
```

to:

```cpp
	int mode = std::clamp(parts[i].ctype, 0, 6);
```

Replace the `switch` body with:

```cpp
	case 0: signal = hray * 34; break;
	case 1: signal = xray * 34; break;
	case 2: signal = rads * 22; break;
	case 3: signal = ionz * 18; break;
	case 4: signal = ntrn * 34; break;
	case 6: signal = std::max(parts[i].life - FissStage1::RMTR_DECAY_RATE, 0); break;
	default: signal = hray * 24 + xray * 24 + rads * 16 + ionz * 10 + ntrn * 24; break;
```

Then replace:

```cpp
	parts[i].tmp2 = std::clamp(hray + xray + rads + ionz, 0, FissStage1::RMTR_MAX_SIGNAL);
```

with:

```cpp
	parts[i].tmp2 = std::clamp(hray + xray + rads + ionz + ntrn, 0, FissStage1::RMTR_MAX_SIGNAL);
```

- [ ] **Step 3: Run source check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command.

Expected: exit code `0`.

## Task 7: Update README Documentation

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Write the failing documentation check**

Run:

```powershell
$readme = Get-Content -Raw "README.md"
foreach ($name in @("FISS Nuclear Sandbox Mod", "Stage 4", "NABS", "NSLW", "NDIFF", "RABS", "RWAL", "HSINK", "PDMP", "fictional gameplay")) {
  if ($readme -notmatch $name) { throw "RED: README missing $name" }
}
```

Expected before implementation: fails with `RED: README missing FISS Nuclear Sandbox Mod`.

- [ ] **Step 2: Add README mod section**

In `README.md`, insert this section after the introductory paragraph that ends with `The rest of the game is learning what happens next.`:

```markdown
FISS Nuclear Sandbox Mod
===========================================================================

This fork includes a fictional, gameplay-only FISS system. It is not real nuclear engineering, real shielding, real neutron physics, or weapon-design guidance. All constants are arbitrary sandbox tuning values.

Stages:

* Stage 1 adds `FISS`, `NTRN`, `FPRD`, `CMTR`, `HEXP`, `SHOK`, and `DENS`.
* Stage 2 adds fictional pressure tools and controls: `BEXP`, `SEXP`, `PABS`, `PWAL`, and `PDRV`.
* Stage 3 adds fictional radiant-energy and aftermath effects: `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR`.
* Stage 4 adds fictional control materials: `NABS`, `NSLW`, `NDIFF`, `RABS`, `RWAL`, `HSINK`, and `PDMP`.

Stage 4 controls:

* `NABS` absorbs `NTRN` and slowly degrades into inert residue.
* `NSLW` slows `NTRN` for easier observation and tuning.
* `NDIFF` randomises and weakens `NTRN` paths without multiplying particles.
* `RABS` absorbs `HRAY` and `XRAY`, converting them into heat.
* `RWAL` blocks thermal flash line-of-sight and strongly absorbs `HRAY` / `XRAY`.
* `HSINK` pulls bounded heat from nearby particles and stores it as temperature.
* `PDMP` reduces positive pressure spikes without creating pressure.

Important tuning groups live in `src/simulation/elements/FISS.h`: fission thresholds, particle lifetimes, pressure and heat caps, Stage 3 radiation caps, and Stage 4 control-material constants. Per-frame caps limit `NTRN`, `SHOK`, `HRAY`, `XRAY`, `RADS`, `IONZ`, thermal flash events, and `RADS` emissions.

Manual Stage 4 smoke tests:

* Fire `NTRN` at `NABS`; expected absorption and modest absorber heat.
* Fire `NTRN` through `NSLW`; expected reduced velocity.
* Fire `NTRN` through `NDIFF`; expected randomized direction with no new `NTRN`.
* Fire `HRAY` and `XRAY` at `RABS`; expected absorption and heat.
* Put `RWAL` between a `FISS` reaction and a heat-sensitive target; expected much less thermal flash heating behind the wall.
* Place `HSINK` near hot `FPRD` or `RADS`; expected local cooling and sink warming.
* Place `PDMP` near pressure tools; expected reduced positive pressure spikes.
* Spawn many `RADS`; expected varied lifetimes, with some long-lived particles and eventual inert decay.
```

- [ ] **Step 3: Run documentation check**

Run the command from Step 1.

Expected: exits successfully with no output.

## Task 8: Final Verification

**Files:**
- Inspect all Stage 4 files.

- [ ] **Step 1: Run full compile**

Run the compile command from the File Structure section.

Expected: exit code `0`.

- [ ] **Step 2: Run whitespace check**

Run:

```powershell
git diff --check
```

Expected: exit code `0`. CRLF warnings are acceptable if they match existing repository behavior.

- [ ] **Step 3: Run Stage 4 coverage check**

Run:

```powershell
$files = @(
  "src\simulation\elements\FISS.h",
  "src\simulation\elements\FISS.cpp",
  "src\simulation\elements\NTRN.cpp",
  "src\simulation\elements\HRAY.cpp",
  "src\simulation\elements\XRAY.cpp",
  "src\simulation\elements\RADS.cpp",
  "src\simulation\elements\RMTR.cpp",
  "src\simulation\elements\meson.build",
  "README.md"
)
$text = ($files | ForEach-Object { Get-Content -Raw $_ }) -join "`n"
foreach ($name in @("NABS", "NSLW", "NDIFF", "RABS", "RWAL", "HSINK", "PDMP", "HRAY_STAGE4_TEMP", "RADS_LIFE_MIN", "RADS_LIFE_MAX", "TryAbsorbRadiantParticle", "PT_NTRN")) {
  if ($text -notmatch $name) { throw "coverage check missing $name" }
}
foreach ($file in @("NABS.cpp", "NSLW.cpp", "NDIFF.cpp", "RABS.cpp", "RWAL.cpp", "HSINK.cpp", "PDMP.cpp")) {
  if (!(Test-Path "src\simulation\elements\$file")) { throw "coverage check missing file $file" }
}
Write-Output "GREEN: Stage 4 coverage check passed"
```

Expected: prints `GREEN: Stage 4 coverage check passed`.

- [ ] **Step 4: Run scoped safety scan**

Run:

```powershell
$files = @(
  "src/simulation/elements/FISS.h",
  "src/simulation/elements/FISS.cpp",
  "src/simulation/elements/NTRN.cpp",
  "src/simulation/elements/HRAY.cpp",
  "src/simulation/elements/XRAY.cpp",
  "src/simulation/elements/RADS.cpp",
  "src/simulation/elements/RMTR.cpp",
  "src/simulation/elements/NABS.cpp",
  "src/simulation/elements/NSLW.cpp",
  "src/simulation/elements/NDIFF.cpp",
  "src/simulation/elements/RABS.cpp",
  "src/simulation/elements/RWAL.cpp",
  "src/simulation/elements/HSINK.cpp",
  "src/simulation/elements/PDMP.cpp",
  "README.md"
)
$hits = git diff -U0 -- $files | Select-String -Pattern '^\+.*(critical mass|yield|tamper|reflector|fusion|dose|cross-section|isotope|weapon geometry|thermal pulse|blast timing|moderator material|shielding table)'
if ($hits) { $hits; throw "restricted modelling term added" }
Write-Output "GREEN: scoped safety scan passed"
```

Expected: prints `GREEN: scoped safety scan passed`.

- [ ] **Step 5: Manual sandbox tests**

Launch `build\powder.exe` and run:

1. Fire `NTRN` at `NABS`; expected absorption/removal, modest heat, no new `NTRN`.
2. Fire `NTRN` through `NSLW`; expected slower motion and finite life.
3. Fire `NTRN` through `NDIFF`; expected randomized direction/weakening, no new `NTRN`.
4. Fire `HRAY` and `XRAY` at `RABS`; expected absorption and absorber heat.
5. Put `RWAL` between a `FISS` reaction and a heat-sensitive target; expected reduced thermal flash behind `RWAL`.
6. Place `HSINK` near hot `FPRD` or `RADS`; expected local cooling and `HSINK` warming.
7. Place `PDMP` near `HEXP`, `BEXP`, `SEXP`, or `SHOK`; expected reduced positive pressure spikes.
8. Spawn many `RADS`; expected randomized lifetimes with some long-lived particles and eventual inert decay.
9. Place `RMTR` near `NTRN`, `HRAY`, `XRAY`, `RADS`, and `IONZ`; expected radiation and neutron readings without particle alteration.
10. Build a moderate mixed Stage 4 setup; expected no crash, freeze, infinite particles, or unbounded heat/pressure.

- [ ] **Step 6: Commit only when requested**

If the user asks for a commit after implementation and verification:

```powershell
git add README.md src/simulation/elements/FISS.h src/simulation/elements/FISS.cpp src/simulation/elements/NTRN.cpp src/simulation/elements/HRAY.cpp src/simulation/elements/XRAY.cpp src/simulation/elements/RADS.cpp src/simulation/elements/RMTR.cpp src/simulation/elements/meson.build src/simulation/elements/NABS.cpp src/simulation/elements/NSLW.cpp src/simulation/elements/NDIFF.cpp src/simulation/elements/RABS.cpp src/simulation/elements/RWAL.cpp src/simulation/elements/HSINK.cpp src/simulation/elements/PDMP.cpp docs/superpowers/plans/2026-06-18-stage-4-fiss-control-materials.md
git commit -m "feat: add fictional stage 4 FISS control materials"
```
