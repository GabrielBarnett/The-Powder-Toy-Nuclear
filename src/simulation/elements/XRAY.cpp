#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_XRAY()
{
	Identifier = "DEFAULT_PT_XRAY";
	Name = "XRAY";
	Colour = 0x8FB8FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	Weight = -1;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 1;
	Description = "Fictional penetrating energy particle. Creates ionised-looking trails and mild heat.";
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
	if (sim->rng.chance(FissStage1::XRAY_IONZ_CHANCE, 10000))
		FissStage1::TryCreateIONZ(sim, x, y, 1);

	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (!dx && !dy)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (!r)
				continue;
			int type = TYP(r);
			if ((type == PT_RABS || type == PT_RWAL) && FissStage1::TryAbsorbRadiantParticle(sim, i, ID(r), type, false))
				return 1;
			if (type == PT_FISS)
			{
				parts[ID(r)].tmp2 = FissStage1::ClampActivation(parts[ID(r)].tmp2 + FissStage1::XRAY_FISS_ACTIVATION_BONUS);
				continue;
			}
			if (SimulationData::CRef().elements[type].Properties & (TYPE_GAS | TYPE_ENERGY))
				continue;
			if (sim->rng.chance(FissStage1::XRAY_PENETRATION_CHANCE, 10000))
				continue;
			if (sim->rng.chance(FissStage1::XRAY_ABSORPTION_CHANCE, 10000))
			{
				parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp + FissStage1::XRAY_HEAT_DEPOSIT, MIN_TEMP, MAX_TEMP);
				sim->kill_part(i);
				return 1;
			}
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 105;
	*firer = 95;
	*fireg = 150;
	*fireb = 255;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	float speed = float(sim->rng.between(FissStage1::XRAY_SPEED_MIN * 100, FissStage1::XRAY_SPEED_MAX * 100)) / 100.0f;
	sim->parts[i].life = sim->rng.between(FissStage1::XRAY_LIFE_MIN, FissStage1::XRAY_LIFE_MAX);
	sim->parts[i].vx = cosf(angle) * speed;
	sim->parts[i].vy = sinf(angle) * speed;
}
