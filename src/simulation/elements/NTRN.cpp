#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

static void HeatControl(Parts &parts, int r, float amount)
{
	parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp + amount, MIN_TEMP, MAX_TEMP);
}

static void RandomizeVelocity(Simulation *sim, Particle &part, float multiplier)
{
	float speed = std::sqrt(part.vx * part.vx + part.vy * part.vy) * multiplier;
	if (speed < 0.05f)
		speed = 0.05f;
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	part.vx = cosf(angle) * speed;
	part.vy = sinf(angle) * speed;
}

void Element::Element_NTRN()
{
	Identifier = "DEFAULT_PT_NTRN";
	Name = "NTRN";
	Colour = 0x69E6FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.98f;
	Gravity = 0.0f;
	Diffusion = 0.02f;
	HotAir = 0.001f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 40;
	Description = "Fictional neutron particle. Activates compressed FISS probabilistically.";

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
	bool interacted = false;
	for (int dy = -FissStage1::NTRN_INTERACTION_RADIUS; dy <= FissStage1::NTRN_INTERACTION_RADIUS; dy++)
	{
		for (int dx = -FissStage1::NTRN_INTERACTION_RADIUS; dx <= FissStage1::NTRN_INTERACTION_RADIUS; dx++)
		{
			if (dx * dx + dy * dy > FissStage1::NTRN_INTERACTION_RADIUS * FissStage1::NTRN_INTERACTION_RADIUS)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			int type = TYP(r);
			if (type == PT_NABS)
			{
				if (sim->rng.chance(FissStage1::NABS_ABSORPTION_CHANCE, 10000))
				{
					HeatControl(parts, r, FissStage1::NABS_HEAT_ON_ABSORB);
					parts[ID(r)].tmp += FissStage1::NABS_DAMAGE_PER_ABSORB;
					sim->kill_part(i);
					return 1;
				}
				continue;
			}
			if (type == PT_NSLW)
			{
				if (sim->rng.chance(FissStage1::NSLW_SLOW_CHANCE, 10000))
				{
					parts[i].vx *= FissStage1::NSLW_SPEED_MULTIPLIER;
					parts[i].vy *= FissStage1::NSLW_SPEED_MULTIPLIER;
					parts[i].life = std::max(parts[i].life + FissStage1::NSLW_LIFE_CHANGE, 1);
					HeatControl(parts, r, FissStage1::NSLW_HEAT_ON_INTERACTION);
					interacted = true;
				}
				continue;
			}
			if (type == PT_NDIFF)
			{
				if (sim->rng.chance(FissStage1::NDIFF_ABSORPTION_CHANCE, 10000))
				{
					sim->kill_part(i);
					return 1;
				}
				if (sim->rng.chance(FissStage1::NDIFF_SCATTER_CHANCE, 10000))
				{
					RandomizeVelocity(sim, parts[i], FissStage1::NDIFF_SPEED_MULTIPLIER);
					parts[i].life = std::max(parts[i].life - FissStage1::NDIFF_LIFE_PENALTY, 1);
					interacted = true;
				}
				continue;
			}
			if (type != PT_FISS)
				continue;
			Particle &fiss = parts[ID(r)];
			fiss.tmp2 = FissStage1::ClampActivation(fiss.tmp2 + FissStage1::ACTIVATION_GAIN / 2);
			if (fiss.tmp > 0 && sim->rng.chance(1, 4))
				fiss.tmp = FissStage1::ClampCompression(fiss.tmp + 1);
			interacted = true;
		}
	}
	if (interacted && sim->rng.chance(FissStage1::NTRN_ABSORPTION_CHANCE, 10000))
	{
		sim->kill_part(i);
		return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 90;
	*firer = 35;
	*fireg = 165;
	*fireb = 210;
	*pixel_mode |= FIRE_ADD | PMODE_ADD;
	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
	float speed = float(sim->rng.between(FissStage1::NTRN_SPEED_MIN * 100, FissStage1::NTRN_SPEED_MAX * 100)) / 100.0f;
	sim->parts[i].life = sim->rng.between(FissStage1::NTRN_LIFE_MIN, FissStage1::NTRN_LIFE_MAX);
	sim->parts[i].vx = cosf(angle) * speed;
	sim->parts[i].vy = sinf(angle) * speed;
}
