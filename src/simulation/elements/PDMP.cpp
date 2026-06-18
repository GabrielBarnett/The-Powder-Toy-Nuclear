#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_PDMP()
{
	Identifier = "DEFAULT_PT_PDMP";
	Name = "PDMP";
	Colour = 0x47505C_rgb;
	MenuVisible = 1;
	MenuSection = SC_FORCE;
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
	Hardness = 38;
	Weight = 100;
	DefaultProperties.temp = R_TEMP + 273.15f;
	HeatConduct = 45;
	Description = "Fictional pressure damper. Reduces pressure spikes without creating pressure.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;
	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;
	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	for (int dy = -FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dy <= FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dy++)
	{
		for (int dx = -FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dx <= FissStage1::PDMP_PRESSURE_SAMPLE_RADIUS; dx++)
		{
			int cx = x / CELL + dx;
			int cy = y / CELL + dy;
			if (!InCellBounds(cx, cy))
				continue;
			float pressure = sim->pv[cy][cx];
			if (pressure <= 0.0f)
				continue;
			sim->pv[cy][cx] = restrict_flt(pressure * FissStage1::PDMP_PRESSURE_DAMPING_RATE, MIN_PRESSURE, MAX_PRESSURE);
			if (pressure > 2.0f)
				parts[i].tmp += FissStage1::PDMP_DAMAGE_PER_PRESSURE;
		}
	}
	if (parts[i].tmp >= FissStage1::PDMP_FAILURE_THRESHOLD)
	{
		sim->part_change_type(i, x, y, PT_BRCK);
		return 1;
	}
	return 0;
}
