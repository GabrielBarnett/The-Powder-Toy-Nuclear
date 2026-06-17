# Stage 1B + Stage 2 FISS Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rebalance `HEXP`/`SHOK`, improve FISS diagnostics and safeguards, and add fictional Stage 2 pressure/compression tools without adding real-world nuclear or weapon modelling.

**Architecture:** Keep the existing Stage 1A element-per-file pattern under `src/simulation/elements/`. Use `FISS.h` as the central tuning/API surface for now, expanding it with Stage 1B/2 constants and shared helpers. Implement new pressure tools as small independent element files registered through `src/simulation/elements/meson.build`, with frequent compile checkpoints.

**Tech Stack:** C++20, Meson/Ninja, The Powder Toy simulation element API, PowerShell verification commands on Windows.

---

## File Structure

- Modify `src/simulation/elements/FISS.h`: add Stage 1B/2 constants and helper declarations.
- Modify `src/simulation/elements/FISS.cpp`: expose frame-cap snapshots, add local activity tracking, and keep shared pressure/heat helpers.
- Modify `src/simulation/elements/HEXP.cpp`: rebalance pressure/heat and remove hot byproduct behavior.
- Modify `src/simulation/elements/SHOK.cpp`: add pressure cap and explicit low/no heat behavior.
- Modify `src/simulation/elements/CMTR.cpp`: improve diagnostics and optional mode behavior.
- Modify `src/simulation/elements/DENS.cpp`: keep inert pressure-material behavior aligned with Stage 2.
- Modify `src/simulation/elements/meson.build`: register `BEXP`, `SEXP`, `PDRV`, `PABS`, and `PWAL`.
- Create `src/simulation/elements/BEXP.cpp`: sharp fictional pressure explosive.
- Create `src/simulation/elements/SEXP.cpp`: slower sustained fictional pressure explosive.
- Create `src/simulation/elements/PDRV.cpp`: abstract directional pressure driver.
- Create `src/simulation/elements/PABS.cpp`: inert pressure damper.
- Create `src/simulation/elements/PWAL.cpp`: inert pressure-resistant material.

Use this compile command after each implementation checkpoint:

```powershell
$env:PATH="C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts;C:\Users\Gabriel\.cache\codex-build-tools\msys64\ucrt64\bin;C:\Users\Gabriel\.cache\codex-build-tools\w64devkit\bin;$env:PATH"
& "C:\Users\Gabriel\.cache\codex-build-tools\meson-ninja-venv\Scripts\meson.exe" compile -C build
```

Expected: exit code `0`, with changed element `.cpp.obj` files compiled and `powder.exe` linked.

## Task 1: HEXP And SHOK Hotfix

**Files:**
- Modify: `src/simulation/elements/FISS.h`
- Modify: `src/simulation/elements/HEXP.cpp`
- Modify: `src/simulation/elements/SHOK.cpp`

- [ ] **Step 1: Write source-level failing checks**

Run:

```powershell
$hexp = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($hexp -notmatch "HEXP_PRESSURE_CAP") { throw "RED: HEXP_PRESSURE_CAP missing" }
if ($hexp -notmatch "HEXP_MAX_TEMP") { throw "RED: HEXP_MAX_TEMP missing" }
if ($hexp -notmatch "SHOK_PRESSURE_CAP") { throw "RED: SHOK_PRESSURE_CAP missing" }
if ($hexp -notmatch "SHOK_HEAT") { throw "RED: SHOK_HEAT missing" }
```

Expected before implementation: command fails with at least `RED: HEXP_PRESSURE_CAP missing`.

- [ ] **Step 2: Add hotfix tuning constants**

In `src/simulation/elements/FISS.h`, replace the existing `HEXP` and `SHOK` constants:

```cpp
	constexpr float HEXP_IGNITION_TEMP = 620.0f + 273.15f;
	constexpr float HEXP_PRESSURE = 7.0f;
	constexpr float HEXP_PRESSURE_CAP = 9.0f;
	constexpr float HEXP_HEAT = 180.0f;
	constexpr float HEXP_MAX_TEMP = 520.0f + 273.15f;
	constexpr int HEXP_SHOK_COUNT = 4;
	constexpr int HEXP_RADIUS = 2;

	constexpr int SHOK_LIFE = 8;
	constexpr float SHOK_PRESSURE = 2.2f;
	constexpr float SHOK_PRESSURE_CAP = 3.0f;
	constexpr float SHOK_HEAT = 0.0f;
	constexpr int SHOK_RADIUS = 1;
	constexpr float SHOK_DECAY = 0.70f;
```

- [ ] **Step 3: Rebalance HEXP update behavior**

In `src/simulation/elements/HEXP.cpp`, replace the detonation block after `if (!triggered) return 0;` with:

```cpp
	// HEXP is a fictional compression driver. It creates pressure for FISS
	// tests, but the high-energy event should come from compressed FISS.
	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::HEXP_RADIUS, FissStage1::HEXP_PRESSURE, FissStage1::HEXP_PRESSURE_CAP);
	FissStage1::AddHeatInRadius(sim, x, y, FissStage1::HEXP_RADIUS * CELL, FissStage1::HEXP_HEAT, FissStage1::HEXP_HEAT);
	for (int s = 0; s < FissStage1::HEXP_SHOK_COUNT; s++)
	{
		if (!FissStage1::TryCreateSHOK(sim, x, y, FissStage1::HEXP_RADIUS * CELL))
			break;
	}

	if (sim->rng.chance(1, 2))
	{
		sim->part_change_type(i, x, y, PT_SMKE);
		parts[i].life = sim->rng.between(40, 90);
		parts[i].temp = restrict_flt(FissStage1::HEXP_MAX_TEMP, MIN_TEMP, MAX_TEMP);
	}
	else
	{
		sim->kill_part(i);
	}
	return 1;
```

- [ ] **Step 4: Rebalance SHOK update behavior**

In `src/simulation/elements/SHOK.cpp`, replace the first line of `update`:

```cpp
	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::SHOK_RADIUS, FissStage1::SHOK_PRESSURE, FissStage1::SHOK_PRESSURE_CAP);
	if (FissStage1::SHOK_HEAT > 0.0f)
		FissStage1::AddHeatInRadius(sim, x, y, FissStage1::SHOK_RADIUS * CELL, FissStage1::SHOK_HEAT, FissStage1::SHOK_HEAT);
```

Keep the adjacent `FISS.tmp` nudge below this block.

- [ ] **Step 5: Run source-level checks again**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "HEXP_PRESSURE_CAP") { throw "HEXP_PRESSURE_CAP missing" }
if ($header -notmatch "HEXP_MAX_TEMP") { throw "HEXP_MAX_TEMP missing" }
if ($header -notmatch "SHOK_PRESSURE_CAP") { throw "SHOK_PRESSURE_CAP missing" }
if ($header -notmatch "SHOK_HEAT") { throw "SHOK_HEAT missing" }
$hexp = Get-Content -Raw "src\simulation\elements\HEXP.cpp"
if ($hexp -match "PT_PLSM") { throw "HEXP still references PT_PLSM byproduct" }
if ($hexp -notmatch "PT_SMKE") { throw "HEXP mild byproduct missing" }
```

Expected: command exits successfully with no output.

- [ ] **Step 6: Compile**

Run the compile command from the File Structure section.

Expected: exit code `0`.

- [ ] **Step 7: Manual smoke test**

Launch `build\powder.exe`.

Manual checks:
- Detonate `HEXP` alone: visible pressure pulse, modest heat, no plasma.
- Detonate `HEXP` near non-FISS materials: no extreme temperature destruction.
- Place `SHOK` near `FISS`: `FISS.tmp` increases with little/no heat.

- [ ] **Step 8: Commit checkpoint if desired**

Only commit if the user explicitly asks. Suggested command:

```powershell
git add src/simulation/elements/FISS.h src/simulation/elements/HEXP.cpp src/simulation/elements/SHOK.cpp
git commit -m "fix: rebalance fictional pressure drivers"
```

## Task 2: Frame Counters And CMTR Diagnostics

**Files:**
- Modify: `src/simulation/elements/FISS.h`
- Modify: `src/simulation/elements/FISS.cpp`
- Modify: `src/simulation/elements/CMTR.cpp`

- [ ] **Step 1: Write source-level failing checks**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "FrameStats") { throw "RED: FrameStats missing" }
if ($header -notmatch "GetFrameStats") { throw "RED: GetFrameStats missing" }
$cmtr = Get-Content -Raw "src\simulation\elements\CMTR.cpp"
if ($cmtr -notmatch "case 2") { throw "RED: CMTR neutron flux mode missing" }
if ($cmtr -notmatch "case 4") { throw "RED: CMTR pressure mode missing" }
```

Expected before implementation: command fails with `RED: FrameStats missing`.

- [ ] **Step 2: Add public frame stats declarations**

In `src/simulation/elements/FISS.h`, add inside `namespace FissStage1` above the helper declarations:

```cpp
	struct FrameStats
	{
		uint64_t frame = 0;
		int fissionEvents = 0;
		int fissNeutronEmissions = 0;
		int ntrnCreated = 0;
		int shokCreated = 0;
		int compressedFissCount = 0;
		int activeFissCount = 0;
	};

	FrameStats GetFrameStats(Simulation *sim);
	void NoteFISSState(Simulation *sim, int compression, int activation);
```

- [ ] **Step 3: Extend the private caps struct**

In `src/simulation/elements/FISS.cpp`, replace `FrameCaps` with:

```cpp
	struct FrameCaps
	{
		uint64_t frame = UINT64_MAX;
		int fissionEvents = 0;
		int fissNeutronEmissions = 0;
		int ntrnCreated = 0;
		int shokCreated = 0;
		int compressedFissCount = 0;
		int activeFissCount = 0;
	};
```

In `CapsFor`, reset the two new counts:

```cpp
			caps.compressedFissCount = 0;
			caps.activeFissCount = 0;
```

- [ ] **Step 4: Implement stats accessors**

In `src/simulation/elements/FISS.cpp`, inside `namespace FissStage1`, add:

```cpp
	FrameStats GetFrameStats(Simulation *sim)
	{
		auto &frameCaps = CapsFor(sim);
		return {
			frameCaps.frame,
			frameCaps.fissionEvents,
			frameCaps.fissNeutronEmissions,
			frameCaps.ntrnCreated,
			frameCaps.shokCreated,
			frameCaps.compressedFissCount,
			frameCaps.activeFissCount,
		};
	}

	void NoteFISSState(Simulation *sim, int compression, int activation)
	{
		auto &frameCaps = CapsFor(sim);
		if (compression >= FISS_NEUTRON_EMISSION_THRESHOLD)
			frameCaps.compressedFissCount++;
		if (activation >= MIN_ACTIVATION_FOR_FISSION)
			frameCaps.activeFissCount++;
	}
```

- [ ] **Step 5: Record FISS state during update**

In `src/simulation/elements/FISS.cpp`, after `parts[i].tmp2 = ...` in `update`, add:

```cpp
	NoteFISSState(sim, parts[i].tmp, parts[i].tmp2);
```

- [ ] **Step 6: Replace CMTR update with mode-aware diagnostics**

In `src/simulation/elements/CMTR.cpp`, replace the body of `update` with:

```cpp
	constexpr int sampleRadius = 6;
	int fissCount = 0;
	int compressionTotal = 0;
	int activationTotal = 0;
	int readyCount = 0;
	int neutronFlux = FissStage1::CountNearbyType(sim, x, y, sampleRadius, PT_NTRN);
	for (int dy = -sampleRadius; dy <= sampleRadius; dy++)
	{
		for (int dx = -sampleRadius; dx <= sampleRadius; dx++)
		{
			if (dx * dx + dy * dy > sampleRadius * sampleRadius)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (TYP(r) != PT_FISS)
				continue;
			const Particle &fiss = parts[ID(r)];
			fissCount++;
			compressionTotal += fiss.tmp;
			activationTotal += fiss.tmp2;
			if (fiss.tmp >= FissStage1::FISS_NEUTRON_EMISSION_THRESHOLD)
				readyCount++;
		}
	}

	int avgCompression = fissCount ? compressionTotal / fissCount : 0;
	int avgActivation = fissCount ? activationTotal / fissCount : 0;
	int pressureReading = int(std::clamp(sim->pv[y / CELL][x / CELL], 0.0f, 255.0f));
	auto stats = FissStage1::GetFrameStats(sim);
	int mode = std::clamp(parts[i].ctype, 0, 4);

	parts[i].tmp = avgCompression;
	parts[i].tmp2 = avgActivation;
	parts[i].life = readyCount;

	switch (mode)
	{
	case 1:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(avgActivation * 8), MIN_TEMP, MAX_TEMP);
		break;
	case 2:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(neutronFlux * 60), MIN_TEMP, MAX_TEMP);
		break;
	case 3:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(stats.fissionEvents * 20), MIN_TEMP, MAX_TEMP);
		break;
	case 4:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(pressureReading * 12), MIN_TEMP, MAX_TEMP);
		break;
	default:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(avgCompression * 8 + avgActivation * 3), MIN_TEMP, MAX_TEMP);
		break;
	}
	return 0;
```

- [ ] **Step 7: Run source-level checks again**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "FrameStats") { throw "FrameStats missing" }
if ($header -notmatch "GetFrameStats") { throw "GetFrameStats missing" }
$cmtr = Get-Content -Raw "src\simulation\elements\CMTR.cpp"
if ($cmtr -notmatch "case 2") { throw "CMTR neutron flux mode missing" }
if ($cmtr -notmatch "case 4") { throw "CMTR pressure mode missing" }
```

Expected: command exits successfully with no output.

- [ ] **Step 8: Compile**

Run the compile command from the File Structure section.

Expected: exit code `0`.

- [ ] **Step 9: Manual diagnostic test**

Launch `build\powder.exe`.

Manual checks:
- Place `CMTR` near idle `FISS`: low reading.
- Compress `FISS` with `SHOK`: `CMTR.tmp`, color, or temp rises.
- Set `CMTR.ctype` to `1`, `2`, `3`, and `4` with the property tool and verify each mode changes with activation, neutron flux, fission activity, or local pressure.

## Task 3: Add BEXP And SEXP

**Files:**
- Modify: `src/simulation/elements/FISS.h`
- Modify: `src/simulation/elements/meson.build`
- Create: `src/simulation/elements/BEXP.cpp`
- Create: `src/simulation/elements/SEXP.cpp`

- [ ] **Step 1: Write source-level failing checks**

Run:

```powershell
if (Test-Path "src\simulation\elements\BEXP.cpp") { throw "BEXP already exists before task" }
if (Test-Path "src\simulation\elements\SEXP.cpp") { throw "SEXP already exists before task" }
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "BEXP_PRESSURE") { throw "RED: BEXP constants missing" }
```

Expected before implementation: command fails with `RED: BEXP constants missing`.

- [ ] **Step 2: Add BEXP and SEXP constants**

In `src/simulation/elements/FISS.h`, add after the `HEXP` constants:

```cpp
	constexpr float BEXP_PRESSURE = 10.0f;
	constexpr float BEXP_PRESSURE_CAP = 12.0f;
	constexpr float BEXP_HEAT = 120.0f;
	constexpr int BEXP_RADIUS = 1;
	constexpr int BEXP_SHOK_COUNT = 3;

	constexpr float SEXP_PRESSURE = 3.0f;
	constexpr float SEXP_PRESSURE_CAP = 5.0f;
	constexpr float SEXP_HEAT = 80.0f;
	constexpr int SEXP_RADIUS = 2;
	constexpr int SEXP_DURATION = 12;
	constexpr int SEXP_SHOK_COUNT = 1;
```

- [ ] **Step 3: Register BEXP and SEXP in Meson**

In `src/simulation/elements/meson.build`, append after `'DENS',`:

```meson
	'BEXP',
	'SEXP',
```

- [ ] **Step 4: Create BEXP.cpp**

Create `src/simulation/elements/BEXP.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_BEXP()
{
	Identifier = "DEFAULT_PT_BEXP";
	Name = "BEXP";
	Colour = 0xD96A42_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
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
	Hardness = 2;
	Weight = 100;
	HeatConduct = 88;
	Description = "Fictional brisant pressure explosive. Sharp compression pulse with modest heat.";
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

static bool TriggeredByNeighbour(int type)
{
	return type == PT_FIRE || type == PT_PLSM || type == PT_SPRK || type == PT_LIGH || type == PT_SHOK;
}

static int update(UPDATE_FUNC_ARGS)
{
	bool triggered = parts[i].temp >= FissStage1::HEXP_IGNITION_TEMP || parts[i].tmp > 0;
	if (!triggered)
	{
		for (int dy = -1; dy <= 1 && !triggered; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				if (!dx && !dy)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (!InBounds(nx, ny))
					continue;
				if (TriggeredByNeighbour(TYP(pmap[ny][nx])))
				{
					triggered = true;
					break;
				}
			}
		}
	}
	if (!triggered)
		return 0;

	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::BEXP_RADIUS, FissStage1::BEXP_PRESSURE, FissStage1::BEXP_PRESSURE_CAP);
	FissStage1::AddHeatInRadius(sim, x, y, FissStage1::BEXP_RADIUS * CELL, FissStage1::BEXP_HEAT, FissStage1::BEXP_HEAT);
	for (int s = 0; s < FissStage1::BEXP_SHOK_COUNT; s++)
	{
		if (!FissStage1::TryCreateSHOK(sim, x, y, FissStage1::BEXP_RADIUS * CELL))
			break;
	}
	sim->kill_part(i);
	return 1;
}
```

- [ ] **Step 5: Create SEXP.cpp**

Create `src/simulation/elements/SEXP.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SEXP()
{
	Identifier = "DEFAULT_PT_SEXP";
	Name = "SEXP";
	Colour = 0xB68B3E_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.92f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 2;
	Weight = 100;
	HeatConduct = 88;
	Description = "Fictional slow pressure explosive. Lower peak compression over a longer pulse.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE | PROP_LIFE_DEC;

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

static bool TriggeredByNeighbour(int type)
{
	return type == PT_FIRE || type == PT_PLSM || type == PT_SPRK || type == PT_LIGH || type == PT_SHOK;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life <= 0)
	{
		bool triggered = parts[i].temp >= FissStage1::HEXP_IGNITION_TEMP || parts[i].tmp > 0;
		for (int dy = -1; dy <= 1 && !triggered; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				if (!dx && !dy)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (InBounds(nx, ny) && TriggeredByNeighbour(TYP(pmap[ny][nx])))
				{
					triggered = true;
					break;
				}
			}
		}
		if (!triggered)
			return 0;
		parts[i].life = FissStage1::SEXP_DURATION;
	}

	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::SEXP_RADIUS, FissStage1::SEXP_PRESSURE, FissStage1::SEXP_PRESSURE_CAP);
	FissStage1::AddHeatInRadius(sim, x, y, FissStage1::SEXP_RADIUS * CELL, FissStage1::SEXP_HEAT, FissStage1::SEXP_HEAT);
	if (parts[i].life % 4 == 0)
	{
		for (int s = 0; s < FissStage1::SEXP_SHOK_COUNT; s++)
		{
			if (!FissStage1::TryCreateSHOK(sim, x, y, FissStage1::SEXP_RADIUS * CELL))
				break;
		}
	}
	if (parts[i].life <= 1)
	{
		sim->part_change_type(i, x, y, PT_SMKE);
		parts[i].life = sim->rng.between(30, 70);
		return 1;
	}
	return 0;
}
```

- [ ] **Step 6: Run source-level checks**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "BEXP_PRESSURE") { throw "BEXP constants missing" }
if ($header -notmatch "SEXP_DURATION") { throw "SEXP constants missing" }
$meson = Get-Content -Raw "src\simulation\elements\meson.build"
if ($meson -notmatch "'BEXP'") { throw "BEXP not registered" }
if ($meson -notmatch "'SEXP'") { throw "SEXP not registered" }
if (!(Test-Path "src\simulation\elements\BEXP.cpp")) { throw "BEXP.cpp missing" }
if (!(Test-Path "src\simulation\elements\SEXP.cpp")) { throw "SEXP.cpp missing" }
```

Expected: command exits successfully with no output.

- [ ] **Step 7: Compile**

Run the compile command from the File Structure section.

Expected: exit code `0`; `BEXP.cpp.obj` and `SEXP.cpp.obj` compile.

- [ ] **Step 8: Manual comparison tests**

Launch `build\powder.exe`.

Manual checks:
- `BEXP` near `FISS`: sharper pressure spike than `HEXP`.
- `SEXP` near `FISS`: lower but longer pressure push than `BEXP`.
- Neither `BEXP` nor `SEXP` directly creates plasma or extreme heat.

## Task 4: Add PABS And PWAL

**Files:**
- Modify: `src/simulation/elements/FISS.h`
- Modify: `src/simulation/elements/meson.build`
- Create: `src/simulation/elements/PABS.cpp`
- Create: `src/simulation/elements/PWAL.cpp`

- [ ] **Step 1: Write source-level failing checks**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "PABS_PRESSURE_DAMPING") { throw "RED: PABS constants missing" }
if ($header -notmatch "PWAL_PRESSURE_RESISTANCE") { throw "RED: PWAL constants missing" }
```

Expected before implementation: command fails with `RED: PABS constants missing`.

- [ ] **Step 2: Add constants**

In `src/simulation/elements/FISS.h`, add:

```cpp
	constexpr float PABS_PRESSURE_DAMPING = 0.72f;
	constexpr float PABS_HEAT_RESISTANCE = 1200.0f;

	constexpr float PWAL_PRESSURE_RESISTANCE = 22.0f;
	constexpr float PWAL_HEAT_RESISTANCE = 2300.0f;
```

- [ ] **Step 3: Register PABS and PWAL**

In `src/simulation/elements/meson.build`, append:

```meson
	'PABS',
	'PWAL',
```

- [ ] **Step 4: Create PABS.cpp**

Create `src/simulation/elements/PABS.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_PABS()
{
	Identifier = "DEFAULT_PT_PABS";
	Name = "PABS";
	Colour = 0x42505A_rgb;
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
	Hardness = 35;
	Weight = 100;
	HeatConduct = 35;
	Description = "Fictional inert pressure damper for sandbox compression tuning.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::PABS_HEAT_RESISTANCE + 273.15f;
	HighTemperatureTransition = PT_LAVA; //@ PABS -> LAVA(PABS)

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->pv[y / CELL][x / CELL] *= FissStage1::PABS_PRESSURE_DAMPING;
	if (parts[i].temp > FissStage1::PABS_HEAT_RESISTANCE + 273.15f)
		parts[i].temp = FissStage1::PABS_HEAT_RESISTANCE + 273.15f;
	return 0;
}
```

- [ ] **Step 5: Create PWAL.cpp**

Create `src/simulation/elements/PWAL.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "simulation/Air.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_PWAL()
{
	Identifier = "DEFAULT_PT_PWAL";
	Name = "PWAL";
	Colour = 0x59646D_rgb;
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
	Hardness = 65;
	Weight = 100;
	HeatConduct = 45;
	Description = "Fictional inert pressure-resistant wall for compression testing.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::PWAL_HEAT_RESISTANCE + 273.15f;
	HighTemperatureTransition = PT_LAVA; //@ PWAL -> LAVA(PWAL)

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->air->bmap_blockair[y / CELL][x / CELL] = 1;
	sim->air->bmap_blockairh[y / CELL][x / CELL] = 0x8;
	if (sim->pv[y / CELL][x / CELL] > FissStage1::PWAL_PRESSURE_RESISTANCE)
		sim->pv[y / CELL][x / CELL] = FissStage1::PWAL_PRESSURE_RESISTANCE;
	if (parts[i].temp > FissStage1::PWAL_HEAT_RESISTANCE + 273.15f)
		parts[i].temp = FissStage1::PWAL_HEAT_RESISTANCE + 273.15f;
	return 0;
}
```

- [ ] **Step 6: Run source-level checks and compile**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "PABS_PRESSURE_DAMPING") { throw "PABS constants missing" }
if ($header -notmatch "PWAL_PRESSURE_RESISTANCE") { throw "PWAL constants missing" }
$meson = Get-Content -Raw "src\simulation\elements\meson.build"
if ($meson -notmatch "'PABS'") { throw "PABS not registered" }
if ($meson -notmatch "'PWAL'") { throw "PWAL not registered" }
```

Expected: command exits successfully with no output.

Run the compile command from the File Structure section.

Expected: exit code `0`; `PABS.cpp.obj` and `PWAL.cpp.obj` compile.

- [ ] **Step 7: Manual pressure-control test**

Launch `build\powder.exe`.

Manual checks:
- `PABS` near a pressure source reduces local pressure spikes.
- `PWAL` contains pressure better than ordinary solids.
- Neither material emits neutrons or triggers `FISS`.

## Task 5: Add PDRV

**Files:**
- Modify: `src/simulation/elements/FISS.h`
- Modify: `src/simulation/elements/meson.build`
- Create: `src/simulation/elements/PDRV.cpp`

- [ ] **Step 1: Write source-level failing check**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "PDRV_PRESSURE") { throw "RED: PDRV constants missing" }
```

Expected before implementation: command fails with `RED: PDRV constants missing`.

- [ ] **Step 2: Add PDRV constants**

In `src/simulation/elements/FISS.h`, add:

```cpp
	constexpr float PDRV_PRESSURE = 4.0f;
	constexpr float PDRV_PRESSURE_CAP = 6.0f;
	constexpr int PDRV_DIRECTION_MODE = 0;
	constexpr int PDRV_SHOK_COUNT = 2;
	constexpr int PDRV_LIFE = 16;
```

- [ ] **Step 3: Register PDRV**

In `src/simulation/elements/meson.build`, append:

```meson
	'PDRV',
```

- [ ] **Step 4: Create PDRV.cpp**

Create `src/simulation/elements/PDRV.cpp`:

```cpp
#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_PDRV()
{
	Identifier = "DEFAULT_PT_PDRV";
	Name = "PDRV";
	Colour = 0x61A6A6_rgb;
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
	Hardness = 20;
	Weight = 100;
	HeatConduct = 40;
	Description = "Fictional directional pressure driver. Abstract sandbox pressure bias, not shaped-charge physics.";
	Properties = TYPE_SOLID | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Create = &create;
}

static Vec2<int> DirectionFromMode(int mode)
{
	switch (mode & 7)
	{
	case 1: return { 1, 0 };
	case 2: return { 0, 1 };
	case 3: return { -1, 0 };
	case 4: return { 0, -1 };
	case 5: return { 1, 1 };
	case 6: return { -1, 1 };
	case 7: return { -1, -1 };
	default: return { 1, -1 };
	}
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life <= 0)
		parts[i].life = FissStage1::PDRV_LIFE;

	Vec2<int> dir = DirectionFromMode(parts[i].tmp ? parts[i].tmp : FissStage1::PDRV_DIRECTION_MODE);
	int targetX = std::clamp(x + dir.X * CELL, 0, XRES - 1);
	int targetY = std::clamp(y + dir.Y * CELL, 0, YRES - 1);
	FissStage1::AddPressureInRadius(sim, targetX, targetY, 1, FissStage1::PDRV_PRESSURE, FissStage1::PDRV_PRESSURE_CAP);

	if (parts[i].life % 5 == 0)
	{
		for (int s = 0; s < FissStage1::PDRV_SHOK_COUNT; s++)
		{
			if (!FissStage1::TryCreateSHOK(sim, targetX, targetY, CELL))
				break;
		}
	}
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].life = FissStage1::PDRV_LIFE;
}
```

- [ ] **Step 5: Run checks and compile**

Run:

```powershell
$header = Get-Content -Raw "src\simulation\elements\FISS.h"
if ($header -notmatch "PDRV_PRESSURE") { throw "PDRV constants missing" }
$meson = Get-Content -Raw "src\simulation\elements\meson.build"
if ($meson -notmatch "'PDRV'") { throw "PDRV not registered" }
if (!(Test-Path "src\simulation\elements\PDRV.cpp")) { throw "PDRV.cpp missing" }
```

Expected: command exits successfully with no output.

Run the compile command from the File Structure section.

Expected: exit code `0`; `PDRV.cpp.obj` compiles.

- [ ] **Step 6: Manual directional-pressure test**

Launch `build\powder.exe`.

Manual checks:
- Place `PDRV` near pressure view and observe pressure bias.
- Set `PDRV.tmp` to direction modes `1`, `2`, `3`, and `4`; pressure bias changes direction.
- Verify this stays abstract and does not encode real device geometry.

## Task 6: Final Stability And Acceptance Pass

**Files:**
- Inspect: all modified and created Stage 1B/2 files.

- [ ] **Step 1: Run full compile**

Run the compile command from the File Structure section.

Expected: exit code `0`; `powder.exe` links.

- [ ] **Step 2: Run whitespace check**

Run:

```powershell
git diff --check
```

Expected: exit code `0`; CRLF warnings are acceptable if they match existing repository behavior.

- [ ] **Step 3: Run source-level scope checks**

Run:

```powershell
rg "critical mass|yield|tamper|reflector|lensing|isotope|gamma|x-ray|lithium|tritium|deuterium" src\simulation\elements\FISS.h src\simulation\elements\*.cpp
```

Expected: no new real-world modelling terms in the Stage 1B/2 implementation. Existing vanilla names elsewhere do not count unless they were added in this work.

- [ ] **Step 4: Manual acceptance test pass**

Launch `build\powder.exe` and run:
- HEXP hotfix test.
- SHOK compression test.
- BEXP vs HEXP comparison.
- SEXP vs BEXP comparison.
- FISS reaction-source test.
- CMTR diagnostics test.
- PABS/PWAL/DENS pressure-control test.
- Moderate setup stability test.

Expected:
- `FISS` remains the main high-energy source.
- Pressure tools do not create plasma or absurd heat by themselves.
- No crash, freeze, unlimited spawning, or unbounded pressure/heat.

- [ ] **Step 5: Prepare final implementation report**

Report:
- files changed,
- elements modified,
- elements added,
- constants added or changed,
- how `HEXP` was rebalanced,
- how to tune pressure without increasing heat,
- manual tests run and results,
- known limitations.

- [ ] **Step 6: Commit only when requested**

Suggested command if the user asks for a commit:

```powershell
git add src/simulation/elements/FISS.h src/simulation/elements/FISS.cpp src/simulation/elements/HEXP.cpp src/simulation/elements/SHOK.cpp src/simulation/elements/CMTR.cpp src/simulation/elements/DENS.cpp src/simulation/elements/meson.build src/simulation/elements/BEXP.cpp src/simulation/elements/SEXP.cpp src/simulation/elements/PDRV.cpp src/simulation/elements/PABS.cpp src/simulation/elements/PWAL.cpp docs/superpowers/specs/2026-06-17-stage-1b-stage-2-fiss-design.md docs/superpowers/plans/2026-06-17-stage-1b-stage-2-fiss.md
git commit -m "feat: add fictional stage 2 FISS pressure tools"
```
