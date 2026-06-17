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
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->pv[y / CELL][x / CELL] *= FissStage1::PABS_PRESSURE_DAMPING;
	if (parts[i].temp > FissStage1::PABS_HEAT_RESISTANCE + 273.15f)
		parts[i].temp = FissStage1::PABS_HEAT_RESISTANCE + 273.15f;
	return 0;
}
