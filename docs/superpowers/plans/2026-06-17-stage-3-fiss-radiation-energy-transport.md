# Stage 3 FISS Radiation Energy Transport Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add fictional Stage 3 radiant heat, radiation-like particles, lingering debris, ionised gas visuals, and diagnostics to the compression-driven `FISS` system.

**Architecture:** Keep Stage 3 as an extension of the existing `FissStage1` tuning/helper surface. Add capped helper functions in `FISS.cpp`, new focused element files for `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR`, then integrate emissions into `FISS` and low-level aftermath into `FPRD`. `GAMM` stays out of scope.

**Tech Stack:** C++20, The Powder Toy element API, Meson/Ninja, PowerShell verification commands on Windows.

---

## File Structure

- Modify `src/simulation/elements/FISS.h`: add Stage 3 constants and helper declarations.
- Modify `src/simulation/elements/FISS.cpp`: extend frame caps, add capped Stage 3 creation helpers, add thermal flash, and reorder `PerformFissionEvent`.
- Modify `src/simulation/elements/FPRD.cpp`: add weak `XRAY` and optional `RADS` aftermath behavior.
- Modify `src/simulation/elements/meson.build`: register `HRAY`, `XRAY`, `RADS`, `IONZ`, and `RMTR`.
- Create `src/simulation/elements/HRAY.cpp`: fast fictional radiant heat particle.
- Create `src/simulation/elements/XRAY.cpp`: fast fictional penetrating energy particle.
- Create `src/simulation/elements/RADS.cpp`: lingering fictional hot debris.
- Create `src/simulation/elements/IONZ.cpp`: short-lived excited gas/energy visual.
- Create `src/simulation/elements/RMTR.cpp`: Stage 3 radiation diagnostic meter.

Use this compile command after each implementation checkpoint:

```powershell
$env:PATH="C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts;C:\Users\Gabriel\.cache\codex-build-tools\msys64\ucrt64\bin;C:\Users\Gabriel\.cache\codex-build-tools\w64devkit\bin;$env:PATH"
& "C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts\meson.exe" compile -C build
```

Expected: exit code `0`. If the linker reports `cannot open output file powder.exe: Permission denied`, close the running `build\powder.exe` process and rerun the same compile command.

## Task 1: Add Stage 3 Tuning Constants And Helper Declarations

**Files:**
- Modify: `src/simulation/elements/FISS.h`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
foreach ($name in @(
  "THERMAL_FLASH_ENABLED",
  "HRAY_LIFE_MIN",
  "XRAY_LIFE_MIN",
  "RADS_START_TEMP",
  "IONZ_START_TEMP",
  "RMTR_SAMPLE_RADIUS",
  "TryCreateHRAY",
  "TryCreateXRAY",
  "TryCreateRADS",
  "TryCreateIONZ",
  "ApplyThermalFlash"
)) {
  if ($header -notmatch $name) { throw "RED: $name missing" }
}
```

Expected before implementation: fails with `RED: THERMAL_FLASH_ENABLED missing`.

- [ ] **Step 2: Add constants**

In `src/simulation/elements/FISS.h`, insert this block after the existing `PDRV` constants:

```cpp
	constexpr bool THERMAL_FLASH_ENABLED = true;
	constexpr int THERMAL_FLASH_RADIUS = 20;
	constexpr float THERMAL_FLASH_INTENSITY = 360.0f;
	constexpr float THERMAL_FLASH_SURFACE_BONUS = 1.65f;
	constexpr float THERMAL_FLASH_DISTANCE_FALLOFF = 0.055f;
	constexpr float THERMAL_FLASH_MAX_HEAT = 720.0f;
	constexpr int THERMAL_FLASH_LINE_OF_SIGHT_STEPS = 24;
	constexpr int MAX_THERMAL_FLASH_EVENTS_PER_FRAME = 80;

	constexpr int HRAY_LIFE_MIN = 5;
	constexpr int HRAY_LIFE_MAX = 11;
	constexpr int HRAY_SPEED_MIN = 8;
	constexpr int HRAY_SPEED_MAX = 15;
	constexpr float HRAY_HEAT_DEPOSIT = 260.0f;
	constexpr float HRAY_SURFACE_HEAT_MULTIPLIER = 1.40f;
	constexpr int HRAY_ABSORPTION_CHANCE = 7200;
	constexpr int HRAY_PENETRATION_CHANCE = 1800;
	constexpr int HRAY_IONZ_CHANCE = 900;
	constexpr int MAX_HRAY_CREATED_PER_FRAME = 520;

	constexpr int XRAY_LIFE_MIN = 8;
	constexpr int XRAY_LIFE_MAX = 16;
	constexpr int XRAY_SPEED_MIN = 9;
	constexpr int XRAY_SPEED_MAX = 17;
	constexpr float XRAY_HEAT_DEPOSIT = 95.0f;
	constexpr int XRAY_IONZ_CHANCE = 1500;
	constexpr int XRAY_ABSORPTION_CHANCE = 4200;
	constexpr int XRAY_PENETRATION_CHANCE = 4200;
	constexpr int XRAY_FISS_ACTIVATION_BONUS = 4;
	constexpr int MAX_XRAY_CREATED_PER_FRAME = 420;

	constexpr float RADS_START_TEMP = 980.0f + 273.15f;
	constexpr int RADS_START_LIFE = 520;
	constexpr float RADS_COOLING_RATE = 1.8f;
	constexpr int RADS_XRAY_EMISSION_CHANCE = 18;
	constexpr int RADS_NTRN_EMISSION_CHANCE = 2;
	constexpr int RADS_IONZ_CHANCE = 35;
	constexpr int RADS_DECAY_TIME = 1;
	constexpr int MAX_RADS_CREATED_PER_FRAME = 220;
	constexpr int MAX_RADS_EMISSIONS_PER_FRAME = 140;

	constexpr float IONZ_START_TEMP = 420.0f + 273.15f;
	constexpr int IONZ_LIFE = 22;
	constexpr float IONZ_COOLING_RATE = 8.0f;
	constexpr int IONZ_FADE_RATE = 1;
	constexpr float IONZ_PLASMA_TEMP_THRESHOLD = 2200.0f + 273.15f;
	constexpr int MAX_IONZ_CREATED_PER_FRAME = 700;

	constexpr int RMTR_SAMPLE_RADIUS = 7;
	constexpr int RMTR_DECAY_RATE = 6;
	constexpr int RMTR_MAX_SIGNAL = 255;

	constexpr int FISSION_HRAY_CHANCE = 8500;
	constexpr int FISSION_HRAY_MIN = 8;
	constexpr int FISSION_HRAY_MAX = 22;
	constexpr int FISSION_HRAY_RADIUS = 3;
	constexpr int FISSION_XRAY_CHANCE = 6200;
	constexpr int FISSION_XRAY_MIN = 4;
	constexpr int FISSION_XRAY_MAX = 14;
	constexpr int FISSION_XRAY_RADIUS = 4;
	constexpr int FISSION_RADS_CHANCE = 5000;
	constexpr int FISSION_RADS_MIN = 1;
	constexpr int FISSION_RADS_MAX = 5;
	constexpr int FISSION_IONZ_CHANCE = 6500;
	constexpr int FISSION_IONZ_RADIUS = 5;

	constexpr int FPRD_XRAY_EMISSION_CHANCE = 14;
	constexpr int FPRD_RADS_DECAY_CHANCE = 1800;
	constexpr int FPRD_TO_RADS_CHANCE = 2400;
	constexpr int FPRD_TO_ASH_TIME = 1;
```

- [ ] **Step 3: Add declarations**

In `src/simulation/elements/FISS.h`, insert these declarations before `ApplyFISSChainKick`:

```cpp
	bool TryCreateHRAY(Simulation *sim, int x, int y, int radius);
	bool TryCreateXRAY(Simulation *sim, int x, int y, int radius);
	bool TryCreateRADS(Simulation *sim, int x, int y, int radius);
	bool TryCreateIONZ(Simulation *sim, int x, int y, int radius);
	void ApplyThermalFlash(Simulation *sim, int x, int y, int radius, float intensity);
```

- [ ] **Step 4: Run the source check again**

Run the command from Step 1.

Expected: exits successfully with no output.

- [ ] **Step 5: Compile**

Run the compile command from the File Structure section.

Expected: exit code `0`.

## Task 2: Add Shared Stage 3 Caps And Creation Helpers

**Files:**
- Modify: `src/simulation/elements/FISS.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$fiss = Get-Content -Raw "src\simulation\elements\FISS.cpp"
foreach ($name in @(
  "hrayCreated",
  "xrayCreated",
  "radsCreated",
  "ionzCreated",
  "thermalFlashEvents",
  "radsEmissions",
  "GiveStage3Velocity",
  "TryCreateHRAY",
  "TryCreateXRAY",
  "TryCreateRADS",
  "TryCreateIONZ",
  "ApplyThermalFlash"
)) {
  if ($fiss -notmatch $name) { throw "RED: $name missing" }
}
```

Expected before implementation: fails with `RED: hrayCreated missing`.

- [ ] **Step 2: Extend `FrameCaps`**

In `src/simulation/elements/FISS.cpp`, add these fields to `FrameCaps` after `activeFissCount`:

```cpp
		int hrayCreated = 0;
		int xrayCreated = 0;
		int radsCreated = 0;
		int ionzCreated = 0;
		int thermalFlashEvents = 0;
		int radsEmissions = 0;
```

Then reset them inside `CapsFor` when the frame changes:

```cpp
			caps.hrayCreated = 0;
			caps.xrayCreated = 0;
			caps.radsCreated = 0;
			caps.ionzCreated = 0;
			caps.thermalFlashEvents = 0;
			caps.radsEmissions = 0;
```

- [ ] **Step 3: Add local utility helpers**

In the anonymous namespace in `src/simulation/elements/FISS.cpp`, after `GiveNTRNVelocity`, add:

```cpp
	bool IsRadiationTransparent(Simulation *sim, int type)
	{
		if (type == 0)
			return true;
		auto properties = sim->elements[type].Properties;
		return properties & (TYPE_GAS | TYPE_ENERGY);
	}

	void GiveStage3Velocity(Simulation *sim, Particle &part, int speedMin, int speedMax)
	{
		float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
		float speed = float(sim->rng.between(speedMin * 100, speedMax * 100)) / 100.0f;
		part.vx = cosf(angle) * speed;
		part.vy = sinf(angle) * speed;
	}
```

- [ ] **Step 4: Add capped creation helpers**

In `namespace FissStage1`, after `TryCreateSHOK`, add:

```cpp
	bool TryCreateHRAY(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.hrayCreated >= MAX_HRAY_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			int h = sim->create_part(-3, nx, ny, PT_HRAY);
			if (h < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			GiveStage3Velocity(sim, sim->parts[h], HRAY_SPEED_MIN, HRAY_SPEED_MAX);
			sim->parts[h].life = sim->rng.between(HRAY_LIFE_MIN, HRAY_LIFE_MAX);
			frameCaps.hrayCreated++;
			return true;
		}
		return false;
	}

	bool TryCreateXRAY(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.xrayCreated >= MAX_XRAY_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			int r = sim->create_part(-3, nx, ny, PT_XRAY);
			if (r < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			GiveStage3Velocity(sim, sim->parts[r], XRAY_SPEED_MIN, XRAY_SPEED_MAX);
			sim->parts[r].life = sim->rng.between(XRAY_LIFE_MIN, XRAY_LIFE_MAX);
			frameCaps.xrayCreated++;
			return true;
		}
		return false;
	}

	bool TryCreateRADS(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.radsCreated >= MAX_RADS_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			int r = sim->create_part(-3, nx, ny, PT_RADS);
			if (r < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			sim->parts[r].life = RADS_START_LIFE;
			sim->parts[r].temp = restrict_flt(RADS_START_TEMP, MIN_TEMP, MAX_TEMP);
			frameCaps.radsCreated++;
			return true;
		}
		return false;
	}

	bool TryCreateIONZ(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.ionzCreated >= MAX_IONZ_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			if (sim->pmap[ny][nx] && !IsRadiationTransparent(sim, TYP(sim->pmap[ny][nx])))
				continue;
			int r = sim->create_part(-3, nx, ny, PT_IONZ);
			if (r < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			sim->parts[r].life = IONZ_LIFE;
			sim->parts[r].temp = restrict_flt(IONZ_START_TEMP, MIN_TEMP, MAX_TEMP);
			frameCaps.ionzCreated++;
			return true;
		}
		return false;
	}
```

- [ ] **Step 5: Add thermal flash helper**

Still in `namespace FissStage1`, after the capped creation helpers, add:

```cpp
	void ApplyThermalFlash(Simulation *sim, int x, int y, int radius, float intensity)
	{
		if (!THERMAL_FLASH_ENABLED)
			return;
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.thermalFlashEvents >= MAX_THERMAL_FLASH_EVENTS_PER_FRAME)
			return;
		frameCaps.thermalFlashEvents++;

		for (int dy = -radius; dy <= radius; dy++)
		{
			for (int dx = -radius; dx <= radius; dx++)
			{
				int dist2 = dx * dx + dy * dy;
				if (!dist2 || dist2 > radius * radius)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (!InBounds(nx, ny))
					continue;

				int target = sim->pmap[ny][nx];
				if (!target)
					target = sim->photons[ny][nx];
				if (!target)
					continue;
				int targetType = TYP(target);
				if (IsRadiationTransparent(sim, targetType))
					continue;

				bool blocked = false;
				for (int step = 1; step <= THERMAL_FLASH_LINE_OF_SIGHT_STEPS; step++)
				{
					float t = float(step) / float(THERMAL_FLASH_LINE_OF_SIGHT_STEPS + 1);
					int sx = x + int(std::round(float(dx) * t));
					int sy = y + int(std::round(float(dy) * t));
					if (!InBounds(sx, sy))
						break;
					int blocker = sim->pmap[sy][sx];
					if (!blocker)
						continue;
					if (ID(blocker) == ID(target))
						break;
					if (!IsRadiationTransparent(sim, TYP(blocker)))
					{
						blocked = true;
						break;
					}
				}
				if (blocked)
					continue;

				float distance = std::sqrt(float(dist2));
				float falloff = 1.0f / (1.0f + distance * THERMAL_FLASH_DISTANCE_FALLOFF);
				float heat = std::min(intensity * falloff * THERMAL_FLASH_SURFACE_BONUS, THERMAL_FLASH_MAX_HEAT);
				sim->parts[ID(target)].temp = restrict_flt(sim->parts[ID(target)].temp + heat, MIN_TEMP, MAX_TEMP);
			}
		}
	}
```

- [ ] **Step 6: Run the source check again**

Run the command from Step 1.

Expected: exits successfully with no output.

- [ ] **Step 7: Defer compile until all Stage 3 element files exist**

Do not compile immediately after this task because `PT_HRAY`, `PT_XRAY`, `PT_RADS`, and `PT_IONZ` are generated only after the element names are registered, and registration must wait until all five Stage 3 `.cpp` files exist.

## Task 3: Add HRAY And XRAY Element Files

**Files:**
- Create: `src/simulation/elements/HRAY.cpp`
- Create: `src/simulation/elements/XRAY.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
if (!(Test-Path "src\simulation\elements\HRAY.cpp")) { throw "RED: HRAY.cpp missing" }
if (!(Test-Path "src\simulation\elements\XRAY.cpp")) { throw "RED: XRAY.cpp missing" }
```

Expected before implementation: fails with `RED: HRAY.cpp missing`.

- [ ] **Step 2: Create HRAY.cpp**

Create `src/simulation/elements/HRAY.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_HRAY()
{
	Identifier = "DEFAULT_PT_HRAY";
	Name = "HRAY";
	Colour = 0xFFB45A_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	Weight = -1;
	DefaultProperties.temp = FissStage1::IONZ_START_TEMP;
	HeatConduct = 1;
	Description = "Fictional heat ray. Fast radiant heat effect with no pressure or neutron emission.";
	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

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
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (sim->rng.chance(FissStage1::HRAY_IONZ_CHANCE, 10000))
		FissStage1::TryCreateIONZ(sim, x, y, 1);

	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (!dx && !dy)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (!r)
				continue;
			int type = TYP(r);
			if (elements[type].Properties & (TYPE_GAS | TYPE_ENERGY))
				continue;
			if (sim->rng.chance(FissStage1::HRAY_PENETRATION_CHANCE, 10000))
				continue;
			if (sim->rng.chance(FissStage1::HRAY_ABSORPTION_CHANCE, 10000))
			{
				parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp + FissStage1::HRAY_HEAT_DEPOSIT * FissStage1::HRAY_SURFACE_HEAT_MULTIPLIER, MIN_TEMP, MAX_TEMP);
				sim->kill_part(i);
				return 1;
			}
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 130;
	*firer = 255;
	*fireg = 175;
	*fireb = 65;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	float speed = float(sim->rng.between(FissStage1::HRAY_SPEED_MIN * 100, FissStage1::HRAY_SPEED_MAX * 100)) / 100.0f;
	sim->parts[i].life = sim->rng.between(FissStage1::HRAY_LIFE_MIN, FissStage1::HRAY_LIFE_MAX);
	sim->parts[i].vx = cosf(angle) * speed;
	sim->parts[i].vy = sinf(angle) * speed;
}
```

- [ ] **Step 3: Create XRAY.cpp**

Create `src/simulation/elements/XRAY.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_XRAY()
{
	Identifier = "DEFAULT_PT_XRAY";
	Name = "XRAY";
	Colour = 0x8FB8FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	Weight = -1;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 1;
	Description = "Fictional penetrating energy particle. Creates ionised-looking trails and mild heat.";
	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

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
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (sim->rng.chance(FissStage1::XRAY_IONZ_CHANCE, 10000))
		FissStage1::TryCreateIONZ(sim, x, y, 1);

	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (!dx && !dy)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (!r)
				continue;
			int type = TYP(r);
			if (type == PT_FISS)
			{
				parts[ID(r)].tmp2 = FissStage1::ClampActivation(parts[ID(r)].tmp2 + FissStage1::XRAY_FISS_ACTIVATION_BONUS);
				continue;
			}
			if (elements[type].Properties & (TYPE_GAS | TYPE_ENERGY))
				continue;
			if (sim->rng.chance(FissStage1::XRAY_PENETRATION_CHANCE, 10000))
				continue;
			if (sim->rng.chance(FissStage1::XRAY_ABSORPTION_CHANCE, 10000))
			{
				parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp + FissStage1::XRAY_HEAT_DEPOSIT, MIN_TEMP, MAX_TEMP);
				sim->kill_part(i);
				return 1;
			}
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 105;
	*firer = 95;
	*fireg = 150;
	*fireb = 255;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	float speed = float(sim->rng.between(FissStage1::XRAY_SPEED_MIN * 100, FissStage1::XRAY_SPEED_MAX * 100)) / 100.0f;
	sim->parts[i].life = sim->rng.between(FissStage1::XRAY_LIFE_MIN, FissStage1::XRAY_LIFE_MAX);
	sim->parts[i].vx = cosf(angle) * speed;
	sim->parts[i].vy = sinf(angle) * speed;
}
```

- [ ] **Step 4: Run source check**

Run the source check from Step 1.

Expected: exits successfully with no output.

- [ ] **Step 5: Defer compile until Task 4**

Do not compile yet. `HRAY.cpp` and `XRAY.cpp` reference generated element IDs that will not exist until Task 4 registers all five Stage 3 elements.

## Task 4: Register Stage 3 Elements And Add RADS, IONZ, And RMTR

**Files:**
- Modify: `src/simulation/elements/meson.build`
- Create: `src/simulation/elements/RADS.cpp`
- Create: `src/simulation/elements/IONZ.cpp`
- Create: `src/simulation/elements/RMTR.cpp`

- [ ] **Step 1: Write the failing source check**

Run:

```powershell
$meson = Get-Content -Raw "src\simulation\elements\meson.build"
foreach ($name in @("'HRAY'", "'XRAY'", "'RADS'", "'IONZ'", "'RMTR'")) {
  if ($meson -notmatch [regex]::Escape($name)) { throw "RED: $name not registered" }
}
foreach ($file in @("HRAY.cpp", "XRAY.cpp", "RADS.cpp", "IONZ.cpp", "RMTR.cpp")) {
  if (!(Test-Path "src\simulation\elements\$file")) { throw "RED: $file missing" }
}
```

Expected before implementation: fails with `RED: 'HRAY' not registered`.

- [ ] **Step 2: Register all Stage 3 elements**

In `src/simulation/elements/meson.build`, append after `'PDRV',`:

```meson
	'HRAY',
	'XRAY',
	'RADS',
	'IONZ',
	'RMTR',
```

- [ ] **Step 3: Create RADS.cpp**

Create `src/simulation/elements/RADS.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_RADS()
{
	Identifier = "DEFAULT_PT_RADS";
	Name = "RADS";
	Colour = 0x7D8A35_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.20f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.90f;
	Collision = 0.0f;
	Gravity = 0.40f;
	Diffusion = 0.00f;
	HotAir = 0.001f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 3;
	Weight = 80;
	DefaultProperties.temp = FissStage1::RADS_START_TEMP;
	HeatConduct = 70;
	Description = "Fictional hot residue. Weak lingering energy effects, then decays inert.";
	Properties = TYPE_PART | PROP_RADIOACTIVE | PROP_LIFE_DEC;

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
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	parts[i].temp = restrict_flt(parts[i].temp - FissStage1::RADS_COOLING_RATE, MIN_TEMP, MAX_TEMP);
	if (sim->rng.chance(FissStage1::RADS_XRAY_EMISSION_CHANCE, 10000))
		FissStage1::TryCreateXRAY(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::RADS_NTRN_EMISSION_CHANCE, 10000))
		FissStage1::TryCreateNTRN(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::RADS_IONZ_CHANCE, 10000))
		FissStage1::TryCreateIONZ(sim, x, y, 1);
	if (parts[i].life <= FissStage1::RADS_DECAY_TIME)
	{
		sim->part_change_type(i, x, y, PT_BRCK);
		parts[i].temp = restrict_flt(R_TEMP + 273.15f, MIN_TEMP, MAX_TEMP);
		return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float heat = std::clamp((cpart->temp - (R_TEMP + 273.15f)) / FissStage1::RADS_START_TEMP, 0.0f, 1.0f);
	*colr = std::clamp(*colr + int(100.0f * heat), 0, 255);
	*colg = std::clamp(*colg + int(120.0f * heat), 0, 255);
	*firea = int(75.0f * heat);
	*firer = 135;
	*fireg = 180;
	*fireb = 40;
	if (*firea > 0)
		*pixel_mode |= FIRE_ADD;
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].life = FissStage1::RADS_START_LIFE;
	sim->parts[i].temp = FissStage1::RADS_START_TEMP;
}
```

- [ ] **Step 4: Create IONZ.cpp**

Create `src/simulation/elements/IONZ.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_IONZ()
{
	Identifier = "DEFAULT_PT_IONZ";
	Name = "IONZ";
	Colour = 0x6FE7FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.9f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.96f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.08f;
	HotAir = 0.002f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	Weight = 1;
	DefaultProperties.temp = FissStage1::IONZ_START_TEMP;
	HeatConduct = 20;
	Description = "Fictional excited gas glow from radiant energy effects.";
	Properties = TYPE_GAS | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::IONZ_PLASMA_TEMP_THRESHOLD;
	HighTemperatureTransition = PT_PLSM;

	Update = &update;
	Graphics = &graphics;
	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	parts[i].temp = restrict_flt(parts[i].temp - FissStage1::IONZ_COOLING_RATE, MIN_TEMP, MAX_TEMP);
	parts[i].tmp = std::max(parts[i].tmp - FissStage1::IONZ_FADE_RATE, 0);
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int glow = std::clamp(cpart->tmp, 0, 80);
	*firea = 45 + glow;
	*firer = 65;
	*fireg = 210;
	*fireb = 255;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].life = FissStage1::IONZ_LIFE;
	sim->parts[i].tmp = 80;
	sim->parts[i].temp = FissStage1::IONZ_START_TEMP;
}
```

- [ ] **Step 5: Create RMTR.cpp**

Create `src/simulation/elements/RMTR.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_RMTR()
{
	Identifier = "DEFAULT_PT_RMTR";
	Name = "RMTR";
	Colour = 0x385178_rgb;
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
	Hardness = 20;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 35;
	Description = "Radiation diagnostic meter for fictional HRAY, XRAY, RADS, and IONZ activity.";
	Properties = TYPE_SOLID | PROP_NEUTPASS;

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
	int hray = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_HRAY);
	int xray = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_XRAY);
	int rads = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_RADS);
	int ionz = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_IONZ);
	int mode = std::clamp(parts[i].ctype, 0, 5);
	int signal = 0;
	switch (mode)
	{
	case 0: signal = hray * 34; break;
	case 1: signal = xray * 34; break;
	case 2: signal = rads * 22; break;
	case 3: signal = ionz * 18; break;
	case 5: signal = std::max(parts[i].life - FissStage1::RMTR_DECAY_RATE, 0); break;
	default: signal = hray * 28 + xray * 28 + rads * 18 + ionz * 12; break;
	}
	signal = std::clamp(signal, 0, FissStage1::RMTR_MAX_SIGNAL);
	parts[i].tmp = signal;
	parts[i].tmp2 = std::clamp(hray + xray + rads + ionz, 0, FissStage1::RMTR_MAX_SIGNAL);
	parts[i].life = std::max(parts[i].life - FissStage1::RMTR_DECAY_RATE, signal);
	parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(signal * 8), MIN_TEMP, MAX_TEMP);
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float signal = std::clamp(float(cpart->tmp) / float(FissStage1::RMTR_MAX_SIGNAL), 0.0f, 1.0f);
	*colr = 45 + int(130.0f * signal);
	*colg = 70 + int(120.0f * signal);
	*colb = 110 + int(145.0f * signal);
	if (signal > 0.05f)
	{
		*firea = int(90.0f * signal);
		*firer = 80;
		*fireg = 170;
		*fireb = 255;
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
```

- [ ] **Step 6: Run source check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command from the File Structure section.

Expected: exit code `0`; `HRAY.cpp.obj`, `XRAY.cpp.obj`, `RADS.cpp.obj`, `IONZ.cpp.obj`, and `RMTR.cpp.obj` compile.

## Task 5: Integrate Stage 3 Into FISS And FPRD

**Files:**
- Modify: `src/simulation/elements/FISS.cpp`
- Modify: `src/simulation/elements/FPRD.cpp`

- [ ] **Step 1: Write the failing integration check**

Run:

```powershell
$fiss = Get-Content -Raw "src\simulation\elements\FISS.cpp"
foreach ($name in @("ApplyThermalFlash", "FISSION_HRAY_CHANCE", "FISSION_XRAY_CHANCE", "FISSION_RADS_CHANCE", "FISSION_IONZ_CHANCE")) {
  if ($fiss -notmatch $name) { throw "RED: FISS integration missing $name" }
}
$fprd = Get-Content -Raw "src\simulation\elements\FPRD.cpp"
foreach ($name in @("FPRD_XRAY_EMISSION_CHANCE", "FPRD_TO_RADS_CHANCE", "PT_RADS")) {
  if ($fprd -notmatch $name) { throw "RED: FPRD integration missing $name" }
}
```

Expected before implementation: fails with an integration-missing message.

- [ ] **Step 2: Reorder and extend `PerformFissionEvent`**

In `src/simulation/elements/FISS.cpp`, replace the body of `PerformFissionEvent` with:

```cpp
	auto &frameCaps = CapsFor(sim);
	if (frameCaps.fissionEvents >= MAX_FISSION_EVENTS_PER_FRAME)
		return false;
	frameCaps.fissionEvents++;

	sim->part_change_type(i, x, y, PT_FPRD);
	parts[i].temp = restrict_flt(FISSION_FPRD_HEAT, MIN_TEMP, MAX_TEMP);
	parts[i].life = FPRD_START_LIFE;
	parts[i].tmp = 0;
	parts[i].tmp2 = 0;

	ApplyThermalFlash(sim, x, y, THERMAL_FLASH_RADIUS, THERMAL_FLASH_INTENSITY);

	if (Chance10000(sim, FISSION_HRAY_CHANCE))
	{
		int count = sim->rng.between(FISSION_HRAY_MIN, FISSION_HRAY_MAX);
		for (int h = 0; h < count; h++)
		{
			if (!TryCreateHRAY(sim, x, y, FISSION_HRAY_RADIUS))
				break;
		}
	}

	if (Chance10000(sim, FISSION_XRAY_CHANCE))
	{
		int count = sim->rng.between(FISSION_XRAY_MIN, FISSION_XRAY_MAX);
		for (int r = 0; r < count; r++)
		{
			if (!TryCreateXRAY(sim, x, y, FISSION_XRAY_RADIUS))
				break;
		}
	}

	int nCount = sim->rng.between(FISSION_NEUTRON_MIN, FISSION_NEUTRON_MAX);
	for (int n = 0; n < nCount; n++)
	{
		if (!TryCreateNTRN(sim, x, y, FISS_NEUTRON_EMISSION_RADIUS + 1))
			break;
	}

	if (Chance10000(sim, FISSION_RADS_CHANCE))
	{
		int count = sim->rng.between(FISSION_RADS_MIN, FISSION_RADS_MAX);
		for (int r = 0; r < count; r++)
		{
			if (!TryCreateRADS(sim, x, y, FISSION_NEUTRON_EMISSION_RADIUS + 1))
				break;
		}
	}

	if (Chance10000(sim, FISSION_IONZ_CHANCE))
	{
		for (int z = 0; z < FISSION_IONZ_RADIUS; z++)
		{
			if (!TryCreateIONZ(sim, x, y, FISSION_IONZ_RADIUS))
				break;
		}
	}

	AddHeatInRadius(sim, x, y, FISSION_HEAT_RADIUS, FISSION_HEAT_BASE * FISS_ENERGY_MULTIPLIER, MAX_LOCAL_HEAT_FROM_FISS);
	AddPressureInRadius(sim, x, y, FISSION_PRESSURE_RADIUS, FISSION_PRESSURE_BASE * FISS_ENERGY_MULTIPLIER, MAX_LOCAL_PRESSURE_FROM_FISS);

	if (Chance10000(sim, FISSION_SHOK_CHANCE))
	{
		for (int s = 0; s < FISSION_SHOK_COUNT; s++)
		{
			if (!TryCreateSHOK(sim, x, y, FISS_NEUTRON_EMISSION_RADIUS + 1))
				break;
		}
	}

	ApplyFISSChainKick(sim, x, y);
	return true;
```

- [ ] **Step 3: Update FPRD aftermath**

In `src/simulation/elements/FPRD.cpp`, replace `update` with:

```cpp
static int update(UPDATE_FUNC_ARGS)
{
	parts[i].temp = restrict_flt(parts[i].temp - FissStage1::FPRD_COOLING_RATE, MIN_TEMP, MAX_TEMP);
	if (sim->rng.chance(FissStage1::FPRD_XRAY_EMISSION_CHANCE, 10000))
		FissStage1::TryCreateXRAY(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::FPRD_NEUTRON_EMISSION_CHANCE, 10000))
		FissStage1::TryCreateNTRN(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::FPRD_RADS_DECAY_CHANCE, 10000))
		FissStage1::TryCreateRADS(sim, x, y, 1);
	if (parts[i].life <= FissStage1::FPRD_TO_ASH_TIME)
	{
		if (sim->rng.chance(FissStage1::FPRD_TO_RADS_CHANCE, 10000))
		{
			sim->part_change_type(i, x, y, PT_RADS);
			parts[i].life = FissStage1::RADS_START_LIFE;
			parts[i].temp = restrict_flt(FissStage1::RADS_START_TEMP, MIN_TEMP, MAX_TEMP);
		}
		else
		{
			sim->part_change_type(i, x, y, PT_STNE);
			parts[i].temp = restrict_flt(R_TEMP + 273.15f, MIN_TEMP, MAX_TEMP);
		}
		return 1;
	}
	return 0;
}
```

- [ ] **Step 4: Run integration check and compile**

Run the source check from Step 1.

Expected: exits successfully with no output.

Run the compile command from the File Structure section.

Expected: exit code `0`.

## Task 6: Final Verification And Manual Test Handoff

**Files:**
- Inspect all Stage 3 files.

- [ ] **Step 1: Run full compile**

Run the compile command from the File Structure section.

Expected: exit code `0`; `powder.exe` links.

- [ ] **Step 2: Run whitespace check**

Run:

```powershell
git diff --check
```

Expected: exit code `0`. CRLF warnings are acceptable if they match existing repository behavior.

- [ ] **Step 3: Run source-level Stage 3 coverage check**

Run:

```powershell
$required = @(
  "HRAY", "XRAY", "RADS", "IONZ", "RMTR",
  "ApplyThermalFlash",
  "MAX_HRAY_CREATED_PER_FRAME",
  "MAX_XRAY_CREATED_PER_FRAME",
  "MAX_RADS_CREATED_PER_FRAME",
  "MAX_IONZ_CREATED_PER_FRAME"
)
$text = Get-Content -Raw src\simulation\elements\FISS.h,src\simulation\elements\FISS.cpp,src\simulation\elements\FPRD.cpp,src\simulation\elements\meson.build
foreach ($name in $required) {
  if ($text -notmatch $name) { throw "coverage check missing $name" }
}
Write-Output "GREEN: Stage 3 coverage check passed"
```

Expected: prints `GREEN: Stage 3 coverage check passed`.

- [ ] **Step 4: Run scoped safety scan**

Run:

```powershell
$files = @(
  "src/simulation/elements/FISS.h",
  "src/simulation/elements/FISS.cpp",
  "src/simulation/elements/FPRD.cpp",
  "src/simulation/elements/HRAY.cpp",
  "src/simulation/elements/XRAY.cpp",
  "src/simulation/elements/RADS.cpp",
  "src/simulation/elements/IONZ.cpp",
  "src/simulation/elements/RMTR.cpp"
)
$hits = git diff -U0 -- $files | Select-String -Pattern '^\+.*(critical mass|yield|tamper|reflector|fusion|dose|cross-section|isotope|weapon geometry|thermal pulse|blast timing)'
if ($hits) { $hits; throw "restricted modelling term added" }
Write-Output "GREEN: scoped safety scan passed"
```

Expected: prints `GREEN: scoped safety scan passed`.

- [ ] **Step 5: Manual sandbox tests**

Launch `build\powder.exe` and run:
- Spawn `HRAY` in open air: fast, short-lived, no pressure, faint `IONZ` trail.
- Fire `HRAY` at a solid: surface heat/scorch, particle disappears or weakens.
- Trigger `FISS` with an exposed target at distance: thermal flash/`HRAY` heats before `SHOK`.
- Repeat with a wall: wall blocks most flash.
- Spawn `XRAY`: fast, short-lived, `IONZ` trail, no runaway.
- Fire `XRAY` through materials: some pass-through, some absorption, mild heat.
- Trigger compact `FISS`: `HRAY`, `XRAY`, `IONZ`, pressure, heat, `NTRN`, and `FPRD` are visible.
- Observe `RADS` aftermath: active briefly, cools and decays.
- Place `RMTR` near a reaction: signal rises.
- Detonate `HEXP`, `BEXP`, and `SEXP` without `FISS`: no Stage 3 products from pressure tools alone.
- Trigger a moderate compressed setup: no crash, freeze, or unbounded particle creation.

- [ ] **Step 6: Prepare final report**

Report:
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

- [ ] **Step 7: Commit only when requested**

If the user asks for a commit after implementation and verification:

```powershell
git add src/simulation/elements/FISS.h src/simulation/elements/FISS.cpp src/simulation/elements/FPRD.cpp src/simulation/elements/meson.build src/simulation/elements/HRAY.cpp src/simulation/elements/XRAY.cpp src/simulation/elements/RADS.cpp src/simulation/elements/IONZ.cpp src/simulation/elements/RMTR.cpp docs/superpowers/plans/2026-06-17-stage-3-fiss-radiation-energy-transport.md
git commit -m "feat: add fictional stage 3 FISS energy transport"
```
