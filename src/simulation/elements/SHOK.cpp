#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_SHOK()
{
	Identifier = "DEFAULT_PT_SHOK";
	Name = "SHOK";
	Colour = 0xFFE585_rgb;
	MenuVisible = 1;
	MenuSection = SC_FORCE;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = FissStage1::SHOK_DECAY;
	Collision = -0.95f;
	Gravity = 0.0f;
	Diffusion = 0.08f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 1;
	Description = "Temporary fictional shock pulse. Adds pressure for compression testing.";

	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

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
	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::SHOK_RADIUS, FissStage1::SHOK_PRESSURE, FissStage1::SHOK_PRESSURE_CAP);
	if (FissStage1::SHOK_HEAT > 0.0f)
		FissStage1::AddHeatInRadius(sim, x, y, FissStage1::SHOK_RADIUS * CELL, FissStage1::SHOK_HEAT, FissStage1::SHOK_HEAT);
	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (TYP(r) == PT_FISS)
				parts[ID(r)].tmp = FissStage1::ClampCompression(parts[ID(r)].tmp + 3);
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 130;
	*firer = 255;
	*fireg = 210;
	*fireb = 95;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	float speed = float(sim->rng.between(140, 340)) / 100.0f;
	sim->parts[i].life = FissStage1::SHOK_LIFE;
	sim->parts[i].vx = cosf(angle) * speed;
	sim->parts[i].vy = sinf(angle) * speed;
}
