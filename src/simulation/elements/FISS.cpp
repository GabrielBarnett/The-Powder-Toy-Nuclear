#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

namespace
{
	struct FrameCaps
	{
		uint64_t frame = UINT64_MAX;
		int fissionEvents = 0;
		int fissNeutronEmissions = 0;
		int ntrnCreated = 0;
		int shokCreated = 0;
		int compressedFissCount = 0;
		int activeFissCount = 0;
	};

	FrameCaps caps;

	FrameCaps &CapsFor(Simulation *sim)
	{
		if (caps.frame != sim->frameCount)
		{
			caps.frame = sim->frameCount;
			caps.fissionEvents = 0;
			caps.fissNeutronEmissions = 0;
			caps.ntrnCreated = 0;
			caps.shokCreated = 0;
			caps.compressedFissCount = 0;
			caps.activeFissCount = 0;
		}
		return caps;
	}

	bool Chance10000(Simulation *sim, int chance)
	{
		return sim->rng.chance(std::clamp(chance, 0, 10000), 10000);
	}

	void GiveNTRNVelocity(Simulation *sim, Particle &part)
	{
		float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
		float speed = float(sim->rng.between(FissStage1::NTRN_SPEED_MIN * 100, FissStage1::NTRN_SPEED_MAX * 100)) / 100.0f;
		part.vx = cosf(angle) * speed;
		part.vy = sinf(angle) * speed;
		part.life = sim->rng.between(FissStage1::NTRN_LIFE_MIN, FissStage1::NTRN_LIFE_MAX);
	}
}

void Element::Element_FISS()
{
	Identifier = "DEFAULT_PT_FISS";
	Name = "FISS";
	Colour = 0x365A3B_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.35f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.92f;
	Collision = 0.0f;
	Gravity = 0.35f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 8;
	PhotonReflectWavelengths = 0x001FCE00;

	Weight = 92;

	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 130;
	Description = "Fictional fissile material. Stores pressure as compression, then emits fictional neutrons when compressed.";

	Properties = TYPE_PART | PROP_NEUTPASS | PROP_RADIOACTIVE;

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

namespace FissStage1
{
	FrameStats GetFrameStats(Simulation *sim)
	{
		auto &frameCaps = CapsFor(sim);
		return {
			frameCaps.frame,
			frameCaps.fissionEvents,
			frameCaps.fissNeutronEmissions,
			frameCaps.ntrnCreated,
			frameCaps.shokCreated,
			frameCaps.compressedFissCount,
			frameCaps.activeFissCount,
		};
	}

	void NoteFISSState(Simulation *sim, int compression, int activation)
	{
		auto &frameCaps = CapsFor(sim);
		if (compression >= FISS_NEUTRON_EMISSION_THRESHOLD)
			frameCaps.compressedFissCount++;
		if (activation >= MIN_ACTIVATION_FOR_FISSION)
			frameCaps.activeFissCount++;
	}

	int ClampCompression(int value)
	{
		return std::clamp(value, 0, MAX_COMPRESSION);
	}

	int ClampActivation(int value)
	{
		return std::clamp(value, 0, MAX_ACTIVATION);
	}

	int CountNearbyType(Simulation *sim, int x, int y, int radius, int type)
	{
		int count = 0;
		for (int dy = -radius; dy <= radius; dy++)
		{
			for (int dx = -radius; dx <= radius; dx++)
			{
				if (dx * dx + dy * dy > radius * radius)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (!InBounds(nx, ny))
					continue;
				if (TYP(sim->pmap[ny][nx]) == type)
					count++;
				if (TYP(sim->photons[ny][nx]) == type)
					count++;
			}
		}
		return count;
	}

	void AddPressureInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap)
	{
		int cx = x / CELL;
		int cy = y / CELL;
		for (int dy = -radius; dy <= radius; dy++)
		{
			for (int dx = -radius; dx <= radius; dx++)
			{
				if (dx * dx + dy * dy > radius * radius)
					continue;
				int ncx = cx + dx;
				int ncy = cy + dy;
				if (!InCellBounds(ncx, ncy))
					continue;
				float falloff = 1.0f - (float(dx * dx + dy * dy) / float(radius * radius + 1));
				float add = std::min(amount * std::max(falloff, 0.2f), cap);
				sim->pv[ncy][ncx] = restrict_flt(sim->pv[ncy][ncx] + add * CFDS, MIN_PRESSURE, MAX_PRESSURE);
			}
		}
	}

	void AddHeatInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap)
	{
		for (int dy = -radius; dy <= radius; dy++)
		{
			for (int dx = -radius; dx <= radius; dx++)
			{
				if (dx * dx + dy * dy > radius * radius)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (!InBounds(nx, ny))
					continue;
				float falloff = 1.0f - (sqrtf(float(dx * dx + dy * dy)) / float(radius + 1));
				float add = std::min(amount * std::max(falloff, 0.15f), cap);
				int r = sim->pmap[ny][nx];
				if (r)
					sim->parts[ID(r)].temp = restrict_flt(sim->parts[ID(r)].temp + add, MIN_TEMP, MAX_TEMP);
				r = sim->photons[ny][nx];
				if (r)
					sim->parts[ID(r)].temp = restrict_flt(sim->parts[ID(r)].temp + add, MIN_TEMP, MAX_TEMP);
			}
		}
	}

	bool TryCreateNTRN(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.ntrnCreated >= MAX_NTRN_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			int n = sim->create_part(-3, nx, ny, PT_NTRN);
			if (n < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			GiveNTRNVelocity(sim, sim->parts[n]);
			frameCaps.ntrnCreated++;
			return true;
		}
		return false;
	}

	bool TryCreateSHOK(Simulation *sim, int x, int y, int radius)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.shokCreated >= MAX_SHOK_CREATED_PER_FRAME)
			return false;
		for (int attempt = 0; attempt < 8; attempt++)
		{
			int nx = x + sim->rng.between(-radius, radius);
			int ny = y + sim->rng.between(-radius, radius);
			if (!InBounds(nx, ny))
				continue;
			int s = sim->create_part(-3, nx, ny, PT_SHOK);
			if (s < 0)
			{
				if (sim->parts.MaxPartsReached())
					break;
				continue;
			}
			float angle = sim->rng.between(0, 359) * std::numbers::pi_v<float> / 180.0f;
			float speed = float(sim->rng.between(120, 360)) / 100.0f;
			sim->parts[s].vx = cosf(angle) * speed;
			sim->parts[s].vy = sinf(angle) * speed;
			sim->parts[s].life = SHOK_LIFE;
			frameCaps.shokCreated++;
			return true;
		}
		return false;
	}

	void ApplyFISSChainKick(Simulation *sim, int x, int y)
	{
		for (int dy = -FISSION_CHAIN_KICK_RADIUS; dy <= FISSION_CHAIN_KICK_RADIUS; dy++)
		{
			for (int dx = -FISSION_CHAIN_KICK_RADIUS; dx <= FISSION_CHAIN_KICK_RADIUS; dx++)
			{
				if (dx * dx + dy * dy > FISSION_CHAIN_KICK_RADIUS * FISSION_CHAIN_KICK_RADIUS)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (!InBounds(nx, ny))
					continue;
				int r = sim->pmap[ny][nx];
				if (TYP(r) != PT_FISS)
					continue;
				Particle &nearby = sim->parts[ID(r)];
				nearby.tmp = ClampCompression(nearby.tmp + FISSION_CHAIN_KICK_COMPRESSION);
				nearby.tmp2 = ClampActivation(nearby.tmp2 + FISSION_CHAIN_KICK_ACTIVATION);
			}
		}
	}

	void TryEmitCompressionNeutrons(Simulation *sim, int i, int x, int y, Parts &parts)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.fissNeutronEmissions >= MAX_FISS_NEUTRON_EMISSIONS_PER_FRAME || parts[i].tmp < FISS_NEUTRON_EMISSION_THRESHOLD)
			return;
		float ratio = float(parts[i].tmp - FISS_NEUTRON_EMISSION_THRESHOLD) / float(MAX_COMPRESSION - FISS_NEUTRON_EMISSION_THRESHOLD);
		ratio = std::clamp(ratio, 0.0f, 1.0f);
		int chance = FISS_NEUTRON_EMISSION_BASE_CHANCE + int(ratio * FISS_NEUTRON_EMISSION_COMPRESSION_FACTOR);
		if (!Chance10000(sim, chance))
			return;
		int count = sim->rng.between(FISS_NEUTRON_EMISSION_MIN, FISS_NEUTRON_EMISSION_MAX);
		for (int n = 0; n < count; n++)
		{
			if (!TryCreateNTRN(sim, x, y, FISS_NEUTRON_EMISSION_RADIUS))
				break;
		}
		frameCaps.fissNeutronEmissions++;
	}

	bool PerformFissionEvent(Simulation *sim, int i, int x, int y, Parts &parts)
	{
		auto &frameCaps = CapsFor(sim);
		if (frameCaps.fissionEvents >= MAX_FISSION_EVENTS_PER_FRAME)
			return false;
		frameCaps.fissionEvents++;

		sim->part_change_type(i, x, y, PT_FPRD);
		parts[i].temp = restrict_flt(FISSION_FPRD_HEAT, MIN_TEMP, MAX_TEMP);
		parts[i].life = FPRD_START_LIFE;
		parts[i].tmp = 0;
		parts[i].tmp2 = 0;

		AddHeatInRadius(sim, x, y, FISSION_HEAT_RADIUS, FISSION_HEAT_BASE * FISS_ENERGY_MULTIPLIER, MAX_LOCAL_HEAT_FROM_FISS);
		AddPressureInRadius(sim, x, y, FISSION_PRESSURE_RADIUS, FISSION_PRESSURE_BASE * FISS_ENERGY_MULTIPLIER, MAX_LOCAL_PRESSURE_FROM_FISS);

		int nCount = sim->rng.between(FISSION_NEUTRON_MIN, FISSION_NEUTRON_MAX);
		for (int n = 0; n < nCount; n++)
		{
			if (!TryCreateNTRN(sim, x, y, FISS_NEUTRON_EMISSION_RADIUS + 1))
				break;
		}

		if (Chance10000(sim, FISSION_SHOK_CHANCE))
		{
			for (int s = 0; s < FISSION_SHOK_COUNT; s++)
			{
				if (!TryCreateSHOK(sim, x, y, FISS_NEUTRON_EMISSION_RADIUS + 1))
					break;
			}
		}

		ApplyFISSChainKick(sim, x, y);
		return true;
	}
}

static int update(UPDATE_FUNC_ARGS)
{
	using namespace FissStage1;

	float localPressure = sim->pv[y / CELL][x / CELL];
	if (localPressure > PRESSURE_GATE)
		parts[i].tmp = ClampCompression(parts[i].tmp + int(localPressure * COMPRESSION_GAIN));
	else
		parts[i].tmp = ClampCompression(parts[i].tmp - COMPRESSION_DECAY);

	int nearbyNTRN = CountNearbyType(sim, x, y, NTRN_INTERACTION_RADIUS, PT_NTRN);
	parts[i].tmp2 = ClampActivation(parts[i].tmp2 + nearbyNTRN * ACTIVATION_GAIN - ACTIVATION_DECAY);
	NoteFISSState(sim, parts[i].tmp, parts[i].tmp2);

	if (parts[i].tmp >= FISS_NEUTRON_EMISSION_THRESHOLD)
		TryEmitCompressionNeutrons(sim, i, x, y, parts);

	int fissionChance = BASE_FISSION_CHANCE + parts[i].tmp * COMPRESSION_FACTOR + parts[i].tmp2 * ACTIVATION_FACTOR + nearbyNTRN * NEUTRON_FACTOR;
	if (parts[i].tmp >= MIN_COMPRESSION_FOR_FISSION &&
		(parts[i].tmp2 >= MIN_ACTIVATION_FOR_FISSION || nearbyNTRN > 0) &&
		Chance10000(sim, fissionChance))
	{
		if (PerformFissionEvent(sim, i, x, y, parts))
			return 1;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float compression = std::clamp(float(cpart->tmp) / float(FissStage1::MAX_COMPRESSION), 0.0f, 1.0f);
	float activation = std::clamp(float(cpart->tmp2) / float(FissStage1::MAX_ACTIVATION), 0.0f, 1.0f);
	*colr = std::clamp(*colr + int(145.0f * compression), 0, 255);
	*colg = std::clamp(*colg + int(80.0f * activation), 0, 255);
	*colb = std::clamp(*colb + int(120.0f * activation), 0, 255);
	if (cpart->tmp >= FissStage1::FISS_NEUTRON_EMISSION_THRESHOLD)
	{
		*firea = 35 + int(80.0f * compression);
		*firer = 50 + int(155.0f * compression);
		*fireg = 160;
		*fireb = 120 + int(90.0f * activation);
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
