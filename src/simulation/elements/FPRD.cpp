#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_FPRD()
{
	Identifier = "DEFAULT_PT_FPRD";
	Name = "FPRD";
	Colour = 0x6D5841_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.25f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.90f;
	Collision = 0.0f;
	Gravity = 0.45f;
	Diffusion = 0.00f;
	HotAir = 0.001f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 4;

	Weight = 86;

	DefaultProperties.temp = FissStage1::FPRD_START_TEMP;
	HeatConduct = 95;
	Description = "Fictional hot fission-product debris. Cools and weakly emits fictional neutrons.";

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
	parts[i].temp = restrict_flt(parts[i].temp - FissStage1::FPRD_COOLING_RATE, MIN_TEMP, MAX_TEMP);
	if (sim->rng.chance(FissStage1::FPRD_NEUTRON_EMISSION_CHANCE, 10000))
		FissStage1::TryCreateNTRN(sim, x, y, 1);
	if (parts[i].life <= FissStage1::FPRD_DECAY_TIME)
	{
		sim->part_change_type(i, x, y, PT_STNE);
		parts[i].temp = restrict_flt(R_TEMP + 273.15f, MIN_TEMP, MAX_TEMP);
		return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float heat = std::clamp((cpart->temp - (R_TEMP + 273.15f)) / FissStage1::FPRD_START_TEMP, 0.0f, 1.0f);
	*colr = std::clamp(*colr + int(145.0f * heat), 0, 255);
	*colg = std::clamp(*colg + int(70.0f * heat), 0, 255);
	*firea = int(95.0f * heat);
	*firer = 180;
	*fireg = 75;
	*fireb = 30;
	if (*firea > 0)
		*pixel_mode |= FIRE_ADD;
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].life = FissStage1::FPRD_START_LIFE;
	sim->parts[i].temp = FissStage1::FPRD_START_TEMP;
}
