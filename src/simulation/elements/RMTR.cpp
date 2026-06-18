#include "simulation/ElementCommon.h"
#include "FISS.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_RMTR()
{
	Identifier = "DEFAULT_PT_RMTR";
	Name = "RMTR";
	Colour = 0x385178_rgb;
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
	HeatConduct = 35;
	Description = "Radiation diagnostic meter for fictional HRAY, XRAY, RADS, and NTRN activity.";
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
	int hray = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_HRAY);
	int xray = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_XRAY);
	int rads = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_RADS);
	int ntrn = FissStage1::CountNearbyType(sim, x, y, FissStage1::RMTR_SAMPLE_RADIUS, PT_NTRN);
	int mode = std::clamp(parts[i].ctype, 0, 6);
	int signal = 0;
	switch (mode)
	{
	case 0: signal = hray * 34; break;
	case 1: signal = xray * 34; break;
	case 2: signal = rads * 22; break;
	case 3: signal = ntrn * 34; break;
	case 4: signal = ntrn * 34; break;
	case 6: signal = std::max(parts[i].life - FissStage1::RMTR_DECAY_RATE, 0); break;
	default: signal = hray * 24 + xray * 24 + rads * 16 + ntrn * 24; break;
	}
	signal = std::clamp(signal, 0, FissStage1::RMTR_MAX_SIGNAL);
	parts[i].tmp = signal;
	parts[i].tmp2 = std::clamp(hray + xray + rads + ntrn, 0, FissStage1::RMTR_MAX_SIGNAL);
	parts[i].life = std::max(parts[i].life - FissStage1::RMTR_DECAY_RATE, signal);
	parts[i].temp = restrict_flt(R_TEMP + 273.15f + float(signal * 8), MIN_TEMP, MAX_TEMP);
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float signal = std::clamp(float(cpart->tmp) / float(FissStage1::RMTR_MAX_SIGNAL), 0.0f, 1.0f);
	*colr = 45 + int(130.0f * signal);
	*colg = 70 + int(120.0f * signal);
	*colb = 110 + int(145.0f * signal);
	if (signal > 0.05f)
	{
		*firea = int(90.0f * signal);
		*firer = 80;
		*fireg = 170;
		*fireb = 255;
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
