#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_CFIS()
{
	Identifier = "DEFAULT_PT_CFIS";
	Name = "CFIS";
	Colour = 0x496A5C_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.88f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 18;
	PhotonReflectWavelengths = 0x001FCE00;

	Weight = 100;

	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 145;
	Description = "Fictional compressed FISS phase. Holds briefly, synchronises locally, then uses the capped FISS reaction.";

	Properties = TYPE_SOLID | PROP_NEUTPASS | PROP_RADIOACTIVE;

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

static void revertToFiss(Simulation *sim, int i, int x, int y, Parts &parts)
{
	int compression = FissStage1::ClampCompression(parts[i].tmp / 2);
	int activation = FissStage1::ClampActivation(parts[i].tmp2 / 2);
	float temp = parts[i].temp;
	sim->part_change_type(i, x, y, PT_FISS);
	parts[i].tmp = compression;
	parts[i].tmp2 = activation;
	parts[i].tmp3 = 0;
	parts[i].tmp4 = 0;
	parts[i].life = FissStage1::CFIS_FORM_COOLDOWN;
	parts[i].temp = temp;
}

static bool shouldRevertToFiss(Simulation *sim, int i, int x, int y, Parts &parts)
{
	using namespace FissStage1;

	if (parts[i].life <= 0 || parts[i].tmp3 >= CFIS_REVERT_GRACE_FRAMES)
		return false;
	int support = CountNearbyFissCluster(sim, x, y, CFIS_FORM_RADIUS);
	return parts[i].tmp < CFIS_REVERT_TO_FISS_THRESHOLD ||
		(CFIS_REVERT_ON_LOW_NEIGHBOURS && support < CFIS_MIN_NEIGHBOURS_TO_REMAIN);
}

static int syncedCountdown(Simulation *sim, int i, int x, int y, Parts &parts)
{
	int lowestLife = parts[i].life;
	for (int dy = -FissStage1::CFIS_SYNC_RADIUS; dy <= FissStage1::CFIS_SYNC_RADIUS; dy++)
	{
		for (int dx = -FissStage1::CFIS_SYNC_RADIUS; dx <= FissStage1::CFIS_SYNC_RADIUS; dx++)
		{
			if ((!dx && !dy) || dx * dx + dy * dy > FissStage1::CFIS_SYNC_RADIUS * FissStage1::CFIS_SYNC_RADIUS)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = sim->pmap[ny][nx];
			if (TYP(r) != PT_CFIS || ID(r) == i)
				continue;
			if (parts[ID(r)].life > 0)
				lowestLife = std::min(lowestLife, parts[ID(r)].life);
		}
	}
	if (lowestLife < parts[i].life)
	{
		int reduction = std::min(FissStage1::CFIS_SYNC_STRENGTH, FissStage1::CFIS_MAX_SYNC_REDUCTION_PER_FRAME);
		return std::max(lowestLife, parts[i].life - reduction);
	}
	return parts[i].life;
}

static bool chance10000(Simulation *sim, int chance)
{
	return sim->rng.chance(std::clamp(chance, 0, 10000), 10000);
}

static void emitPrefissionXRAY(Simulation *sim, int x, int y)
{
	using namespace FissStage1;

	if (chance10000(sim, CFIS_PREFISSION_XRAY_CHANCE))
	{
		int count = sim->rng.between(CFIS_PREFISSION_XRAY_MIN, CFIS_PREFISSION_XRAY_MAX);
		for (int r = 0; r < count; r++)
		{
			if (!TryUseCFISPrefissionEmissionSlot(sim) || !TryCreateXRAY(sim, x, y, FISSION_XRAY_RADIUS))
				break;
		}
	}
}

static void emitPrefissionHRAY(Simulation *sim, int x, int y)
{
	using namespace FissStage1;

	if (chance10000(sim, CFIS_PREFISSION_HRAY_CHANCE))
	{
		int count = sim->rng.between(CFIS_PREFISSION_HRAY_MIN, CFIS_PREFISSION_HRAY_MAX);
		for (int h = 0; h < count; h++)
		{
			if (!TryUseCFISPrefissionEmissionSlot(sim) || !TryCreateHRAY(sim, x, y, FISSION_HRAY_RADIUS))
				break;
		}
	}

}

static bool tryPerformCFISFission(Simulation *sim, int i, int x, int y, Parts &parts)
{
	using namespace FissStage1;

	bool armedForFission = parts[i].tmp3 >= CFIS_ARMING_DELAY && parts[i].tmp3 >= CFIS_MIN_LIFE_BEFORE_FISSION;
	if (!armedForFission)
	{
		parts[i].life = CFIS_SYNC_MIN_COUNTDOWN;
		return false;
	}
	if (!TryUseCFISClusterFissionSlot(sim))
	{
		parts[i].life = 1;
		return false;
	}
	if (PerformFissionEvent(sim, i, x, y, parts))
		return true;
	parts[i].life = 1;
	return false;
}

static int update(UPDATE_FUNC_ARGS)
{
	using namespace FissStage1;

	if (parts[i].life <= 0)
	{
		if (tryPerformCFISFission(sim, i, x, y, parts))
			return 1;
		return 0;
	}
	parts[i].tmp3 = std::min(parts[i].tmp3 + 1, CFIS_COUNTDOWN_MAX);

	if (CFIS_DAMP_VELOCITY_DURING_COUNTDOWN)
	{
		parts[i].vx *= CFIS_VELOCITY_DAMPING_FACTOR;
		parts[i].vy *= CFIS_VELOCITY_DAMPING_FACTOR;
	}
	if (CFIS_RESIST_PRESSURE_SCATTER && parts[i].tmp3 < CFIS_ARMING_DELAY)
	{
		parts[i].vx = 0.0f;
		parts[i].vy = 0.0f;
	}

	float localPressure = sim->pv[y / CELL][x / CELL];
	if (localPressure > 0.0f)
		parts[i].tmp = ClampCompression(parts[i].tmp + int(localPressure * CFIS_HOLD_STRENGTH));
	else
		parts[i].tmp = ClampCompression(parts[i].tmp - CFIS_COMPRESSION_DECAY);

	int nearbyNTRN = CountNearbyType(sim, x, y, NTRN_INTERACTION_RADIUS, PT_NTRN);
	parts[i].tmp2 = ClampActivation(parts[i].tmp2 + nearbyNTRN * ACTIVATION_GAIN - CFIS_ACTIVATION_DECAY);
	NoteFISSState(sim, parts[i].tmp, parts[i].tmp2);

	if (parts[i].tmp >= FISS_NEUTRON_EMISSION_THRESHOLD)
		TryEmitCompressionNeutrons(sim, i, x, y, parts);

	parts[i].life = syncedCountdown(sim, i, x, y, parts);
	if (parts[i].tmp3 < CFIS_ARMING_DELAY)
		parts[i].life = std::max(parts[i].life, CFIS_SYNC_MIN_COUNTDOWN);

	bool prefissionArmed = !CFIS_PREFISSION_REQUIRE_ARMED || parts[i].tmp3 >= CFIS_ARMING_DELAY;
	if (parts[i].life > 0 && parts[i].tmp3 >= CFIS_XRAY_START_AFTER_FORMATION && prefissionArmed)
		emitPrefissionXRAY(sim, x, y);
	if (parts[i].life > 0 && parts[i].life <= CFIS_HRAY_START_BEFORE_FISSION && prefissionArmed)
		emitPrefissionHRAY(sim, x, y);

	parts[i].life--;
	if (parts[i].life <= 0)
	{
		if (tryPerformCFISFission(sim, i, x, y, parts))
			return 1;
		return 0;
	}

	if (shouldRevertToFiss(sim, i, x, y, parts))
	{
		revertToFiss(sim, i, x, y, parts);
		return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float compression = std::clamp(float(cpart->tmp) / float(FissStage1::MAX_COMPRESSION), 0.0f, 1.0f);
	float countdown = std::clamp(float(FissStage1::CFIS_COUNTDOWN_MAX - cpart->life) / float(FissStage1::CFIS_COUNTDOWN_MAX), 0.0f, 1.0f);
	*colr = std::clamp(*colr + int(95.0f * compression) + int(60.0f * countdown), 0, 255);
	*colg = std::clamp(*colg + int(105.0f * compression), 0, 255);
	*colb = std::clamp(*colb + int(55.0f * countdown), 0, 255);
	*firea = 25 + int(85.0f * compression);
	*firer = 90 + int(120.0f * countdown);
	*fireg = 170;
	*fireb = 105 + int(70.0f * compression);
	*pixel_mode |= FIRE_ADD;
	return 0;
}
