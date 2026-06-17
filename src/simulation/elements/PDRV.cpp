#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_PDRV()
{
	Identifier = "DEFAULT_PT_PDRV";
	Name = "PDRV";
	Colour = 0x61A6A6_rgb;
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
	Hardness = 20;
	Weight = 100;
	HeatConduct = 40;
	Description = "Fictional directional pressure driver. Abstract sandbox pressure bias.";
	Properties = TYPE_SOLID | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Create = &create;
}

static Vec2<int> DirectionFromMode(int mode)
{
	switch (mode & 7)
	{
	case 1: return { 1, 0 };
	case 2: return { 0, 1 };
	case 3: return { -1, 0 };
	case 4: return { 0, -1 };
	case 5: return { 1, 1 };
	case 6: return { -1, 1 };
	case 7: return { -1, -1 };
	default: return { 1, -1 };
	}
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life <= 0)
		parts[i].life = FissStage1::PDRV_LIFE;

	Vec2<int> dir = DirectionFromMode(parts[i].tmp ? parts[i].tmp : FissStage1::PDRV_DIRECTION_MODE);
	int targetX = std::clamp(x + dir.X * CELL, 0, XRES - 1);
	int targetY = std::clamp(y + dir.Y * CELL, 0, YRES - 1);
	FissStage1::AddPressureInRadius(sim, targetX, targetY, 1, FissStage1::PDRV_PRESSURE, FissStage1::PDRV_PRESSURE_CAP);

	if (parts[i].life % 5 == 0)
	{
		for (int s = 0; s < FissStage1::PDRV_SHOK_COUNT; s++)
		{
			if (!FissStage1::TryCreateSHOK(sim, targetX, targetY, CELL))
				break;
		}
	}
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].life = FissStage1::PDRV_LIFE;
}
