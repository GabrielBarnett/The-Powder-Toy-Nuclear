#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_CMTR()
{
	Identifier = "DEFAULT_PT_CMTR";
	Name = "CMTR";
	Colour = 0x406070_rgb;
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
	Hardness = 20;

	Weight = 100;

	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 40;
	Description = "Diagnostic meter for nearby FISS compression and activation.";

	Properties = TYPE_SOLID | PROP_NEUTPASS;

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
	constexpr int sampleRadius = 6;
	int fissCount = 0;
	int compressionTotal = 0;
	int activationTotal = 0;
	int readyCount = 0;
	int neutronFlux = FissStage1::CountNearbyType(sim, x, y, sampleRadius, PT_NTRN);
	for (int dy = -sampleRadius; dy <= sampleRadius; dy++)
	{
		for (int dx = -sampleRadius; dx <= sampleRadius; dx++)
		{
			if (dx * dx + dy * dy > sampleRadius * sampleRadius)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (TYP(r) != PT_FISS)
				continue;
			const Particle &fiss = parts[ID(r)];
			fissCount++;
			compressionTotal += fiss.tmp;
			activationTotal += fiss.tmp2;
			if (fiss.tmp >= FissStage1::FISS_NEUTRON_EMISSION_THRESHOLD)
				readyCount++;
		}
	}

	int avgCompression = fissCount ? compressionTotal / fissCount : 0;
	int avgActivation = fissCount ? activationTotal / fissCount : 0;
	int pressureReading = int(std::clamp(sim->pv[y / CELL][x / CELL], 0.0f, 255.0f));
	auto stats = FissStage1::GetFrameStats(sim);
	int mode = std::clamp(parts[i].ctype, 0, 4);

	parts[i].tmp = avgCompression;
	parts[i].tmp2 = avgActivation;
	parts[i].life = readyCount;

	switch (mode)
	{
	case 1:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(avgActivation * 8), MIN_TEMP, MAX_TEMP);
		break;
	case 2:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(neutronFlux * 60), MIN_TEMP, MAX_TEMP);
		break;
	case 3:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(stats.fissionEvents * 20), MIN_TEMP, MAX_TEMP);
		break;
	case 4:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(pressureReading * 12), MIN_TEMP, MAX_TEMP);
		break;
	default:
		parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(avgCompression * 8 + avgActivation * 3), MIN_TEMP, MAX_TEMP);
		break;
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float compression = std::clamp(float(cpart->tmp) / float(FissStage1::MAX_COMPRESSION), 0.0f, 1.0f);
	float activation = std::clamp(float(cpart->tmp2) / float(FissStage1::MAX_ACTIVATION), 0.0f, 1.0f);
	*colr = 35 + int(210.0f * compression);
	*colg = 70 + int(125.0f * activation);
	*colb = 95 + int(140.0f * activation);
	if (cpart->life > 0)
	{
		*firea = 80;
		*firer = 230;
		*fireg = 210;
		*fireb = 80;
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
