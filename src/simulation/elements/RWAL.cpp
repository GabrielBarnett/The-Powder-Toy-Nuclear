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
