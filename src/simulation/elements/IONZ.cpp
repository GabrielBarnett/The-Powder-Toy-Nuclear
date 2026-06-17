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
