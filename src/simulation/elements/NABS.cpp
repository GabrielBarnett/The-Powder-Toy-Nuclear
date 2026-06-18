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
