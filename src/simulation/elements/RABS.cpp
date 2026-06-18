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
