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
	HighTemperatureTransition = PT_LAVA;

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
