#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HEXP()
{
	Identifier = "DEFAULT_PT_HEXP";
	Name = "HEXP";
	Colour = 0xB24B47_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
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
	Hardness = 2;

	Weight = 100;

	HeatConduct = 88;
	Description = "Fictional high-pressure test explosive for compressing FISS.";

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

static bool TriggeredByNeighbour(int type)
{
	return type == PT_FIRE || type == PT_SPRK || type == PT_LIGH || type == PT_SHOK;
}

static int update(UPDATE_FUNC_ARGS)
{
	bool triggered = parts[i].temp >= FissStage1::HEXP_IGNITION_TEMP || parts[i].tmp > 0;
	if (!triggered)
	{
		for (int dy = -1; dy <= 1 && !triggered; dy++)
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
				if (TriggeredByNeighbour(TYP(r)))
				{
					triggered = true;
					break;
				}
			}
		}
	}
	if (!triggered)
		return 0;

	// HEXP is a fictional compression driver. It creates pressure for FISS
	// tests, but the high-energy event should come from compressed FISS.
	FissStage1::AddPressureInRadius(sim, x, y, FissStage1::HEXP_RADIUS, FissStage1::HEXP_PRESSURE, FissStage1::HEXP_PRESSURE_CAP);
	FissStage1::AddHeatInRadius(sim, x, y, FissStage1::HEXP_RADIUS * CELL, FissStage1::HEXP_HEAT, FissStage1::HEXP_HEAT);
	for (int s = 0; s < FissStage1::HEXP_SHOK_COUNT; s++)
	{
		if (!FissStage1::TryCreateSHOK(sim, x, y, FissStage1::HEXP_RADIUS * CELL))
			break;
	}

	if (sim->rng.chance(1, 2))
	{
		sim->part_change_type(i, x, y, PT_SMKE);
		parts[i].life = sim->rng.between(40, 90);
		parts[i].temp = restrict_flt(FissStage1::HEXP_MAX_TEMP, MIN_TEMP, MAX_TEMP);
	}
	else
	{
		sim->kill_part(i);
	}
	return 1;
}
