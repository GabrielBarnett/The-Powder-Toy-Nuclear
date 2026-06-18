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
	if (sim->rng.chance(FissStage1::RADS_XRAY_EMISSION_CHANCE, 10000) && FissStage1::TryUseRadsEmissionSlot(sim))
		FissStage1::TryCreateXRAY(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::RADS_NTRN_EMISSION_CHANCE, 10000) && FissStage1::TryUseRadsEmissionSlot(sim))
		FissStage1::TryCreateNTRN(sim, x, y, 1);
	if (sim->rng.chance(FissStage1::RADS_IONZ_CHANCE, 10000) && FissStage1::TryUseRadsEmissionSlot(sim))
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
	sim->parts[i].life = FissStage1::RandomRadsLife(sim);
	sim->parts[i].temp = FissStage1::RADS_START_TEMP;
}
