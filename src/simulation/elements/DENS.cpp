#include "simulation/ElementCommon.h"
#include "simulation/Air.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_DENS()
{
	Identifier = "DEFAULT_PT_DENS";
	Name = "DENS";
	Colour = 0x4B5158_rgb;
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
	Hardness = 60;

	Weight = 100;

	HeatConduct = 50;
	Description = "Dense fictional inert solid for pressure confinement tests.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::DENS_HEAT_RESISTANCE + 273.15f;
	HighTemperatureTransition = PT_LAVA; //@ DENS -> LAVA(DENS)

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->air->bmap_blockair[y / CELL][x / CELL] = 1;
	sim->air->bmap_blockairh[y / CELL][x / CELL] = 0x8;
	if (parts[i].temp > FissStage1::DENS_HEAT_RESISTANCE + 273.15f)
		parts[i].temp = FissStage1::DENS_HEAT_RESISTANCE + 273.15f;
	if (sim->pv[y / CELL][x / CELL] > FissStage1::DENS_PRESSURE_RESISTANCE)
		sim->pv[y / CELL][x / CELL] = FissStage1::DENS_PRESSURE_RESISTANCE;
	return 0;
}
