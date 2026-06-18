#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HSINK()
{
	Identifier = "DEFAULT_PT_HSINK";
	Name = "HSNK";
	Colour = 0x31535A_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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
	Hardness = 40;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 95;
	Description = "Fictional heat sink. Pulls bounded heat from nearby particles.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = FissStage1::HSINK_FAILURE_TEMP;
	HighTemperatureTransition = PT_LAVA;
	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	for (int dy = -FissStage1::HSINK_SAMPLE_RADIUS; dy <= FissStage1::HSINK_SAMPLE_RADIUS; dy++)
	{
		for (int dx = -FissStage1::HSINK_SAMPLE_RADIUS; dx <= FissStage1::HSINK_SAMPLE_RADIUS; dx++)
		{
			if (!dx && !dy)
				continue;
			if (dx * dx + dy * dy > FissStage1::HSINK_SAMPLE_RADIUS * FissStage1::HSINK_SAMPLE_RADIUS)
				continue;
			int nx = x + dx;
			int ny = y + dy;
			if (!InBounds(nx, ny))
				continue;
			int r = pmap[ny][nx];
			if (!r || parts[ID(r)].temp <= parts[i].temp)
				continue;
			float pulled = std::min((parts[ID(r)].temp - parts[i].temp) * 0.08f, FissStage1::HSINK_HEAT_ABSORPTION_RATE);
			parts[ID(r)].temp = restrict_flt(parts[ID(r)].temp - pulled, MIN_TEMP, MAX_TEMP);
			parts[i].temp = restrict_flt(parts[i].temp + pulled * 0.55f, MIN_TEMP, MAX_TEMP);
		}
	}
	if (parts[i].temp > FissStage1::HSINK_MAX_SAFE_TEMP)
		parts[i].tmp++;
	return 0;
}
