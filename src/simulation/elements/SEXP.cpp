#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SEXP()
{
	Identifier = "DEFAULT_PT_SEXP";
	Name = "SEXP";
	Colour = 0xB68B3E_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.92f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 2;
	Weight = 100;
	HeatConduct = 88;
	Description = "Fictional slow pressure explosive. Lower peak compression over a longer pulse.";
	Properties = TYPE_SOLID | PROP_NEUTPENETRATE | PROP_LIFE_DEC;

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

static bool TriggeredByNeighbour(int type)
{
	return type == PT_FIRE || type == PT_SPRK || type == PT_LIGH || type == PT_SHOK;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life <= 0)
	{
		bool triggered = parts[i].temp >= FissStage1::HEXP_IGNITION_TEMP || parts[i].tmp > 0;
		for (int dy = -1; dy <= 1 && !triggered; dy++)
		{
			for (int dx = -1; dx <= 1; dx++)
			{
				if (!dx && !dy)
					continue;
				int nx = x + dx;
				int ny = y + dy;
				if (InBounds(nx, ny) && TriggeredByNeighbour(TYP(pmap[ny][nx])))
				{
					triggered = true;
					break;
				}
			}
		}
		if (!triggered)
			return 0;
		parts[i].life = FissStage1::SEXP_DURATION;
	}

	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::SEXP_RADIUS, FissStage1::SEXP_PRESSURE, FissStage1::SEXP_PRESSURE_CAP);
	FissStage1::AddHeatInRadius(sim, x, y, FissStage1::SEXP_RADIUS * CELL, FissStage1::SEXP_HEAT, FissStage1::SEXP_HEAT);
	if (parts[i].life % 4 == 0)
	{
		for (int s = 0; s < FissStage1::SEXP_SHOK_COUNT; s++)
		{
			if (!FissStage1::TryCreateSHOK(sim, x, y, FissStage1::SEXP_RADIUS * CELL))
				break;
		}
	}
	if (parts[i].life <= 1)
	{
		sim->part_change_type(i, x, y, PT_SMKE);
		parts[i].life = sim->rng.between(30, 70);
		return 1;
	}
	return 0;
}
