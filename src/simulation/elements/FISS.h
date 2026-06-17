#pragma once

#include "simulation/ElementDefs.h"

class Simulation;
class Parts;

namespace FissStage1
{
	// Fictional, gameplay-only tuning values. These are not physical constants and
	// deliberately avoid real isotope, explosive, device, or yield modelling.
	constexpr float PRESSURE_GATE = 2.0f;
	constexpr float COMPRESSION_GAIN = 3.5f;
	constexpr int COMPRESSION_DECAY = 2;
	constexpr int MAX_COMPRESSION = 220;

	constexpr int FISS_NEUTRON_EMISSION_THRESHOLD = 70;
	constexpr int FISS_NEUTRON_EMISSION_BASE_CHANCE = 25;
	constexpr int FISS_NEUTRON_EMISSION_COMPRESSION_FACTOR = 625;
	constexpr int FISS_NEUTRON_EMISSION_MIN = 1;
	constexpr int FISS_NEUTRON_EMISSION_MAX = 7;
	constexpr int FISS_NEUTRON_EMISSION_RADIUS = 2;
	constexpr int MAX_FISS_NEUTRON_EMISSIONS_PER_FRAME = 360;

	constexpr int MAX_ACTIVATION = 240;
	constexpr int ACTIVATION_GAIN = 18;
	constexpr int ACTIVATION_DECAY = 1;
	constexpr int MIN_COMPRESSION_FOR_FISSION = 95;
	constexpr int MIN_ACTIVATION_FOR_FISSION = 20;
	constexpr int BASE_FISSION_CHANCE = 1;
	constexpr int COMPRESSION_FACTOR = 2;
	constexpr int ACTIVATION_FACTOR = 3;
	constexpr int NEUTRON_FACTOR = 160;

	constexpr int FISS_ENERGY_MULTIPLIER = 4;
	constexpr float FISSION_PRESSURE_BASE = 3.0f;
	constexpr int FISSION_PRESSURE_RADIUS = 2;
	constexpr float FISSION_HEAT_BASE = 820.0f;
	constexpr int FISSION_HEAT_RADIUS = 5;
	constexpr int FISSION_NEUTRON_MIN = 7;
	constexpr int FISSION_NEUTRON_MAX = 18;
	constexpr int FISSION_SHOK_CHANCE = 4500;
	constexpr int FISSION_SHOK_COUNT = 3;
	constexpr float FISSION_FPRD_HEAT = 3100.0f;
	constexpr int FISSION_CHAIN_KICK_RADIUS = 4;
	constexpr int FISSION_CHAIN_KICK_ACTIVATION = 35;
	constexpr int FISSION_CHAIN_KICK_COMPRESSION = 18;

	constexpr int MAX_FISSION_EVENTS_PER_FRAME = 180;
	constexpr int MAX_NTRN_CREATED_PER_FRAME = 1400;
	constexpr int MAX_SHOK_CREATED_PER_FRAME = 220;
	constexpr float MAX_LOCAL_PRESSURE_FROM_FISS = 28.0f;
	constexpr float MAX_LOCAL_HEAT_FROM_FISS = 4800.0f;

	constexpr int NTRN_LIFE_MIN = 45;
	constexpr int NTRN_LIFE_MAX = 115;
	constexpr int NTRN_INTERACTION_RADIUS = 2;
	constexpr int NTRN_ABSORPTION_CHANCE = 1800;
	constexpr int NTRN_SPEED_MIN = 2;
	constexpr int NTRN_SPEED_MAX = 6;

	constexpr float FPRD_START_TEMP = 2600.0f;
	constexpr int FPRD_START_LIFE = 950;
	constexpr float FPRD_COOLING_RATE = 3.0f;
	constexpr int FPRD_NEUTRON_EMISSION_CHANCE = 7;
	constexpr int FPRD_DECAY_TIME = 1;

	constexpr float HEXP_IGNITION_TEMP = 620.0f + 273.15f;
	constexpr float HEXP_PRESSURE = 7.0f;
	constexpr float HEXP_PRESSURE_CAP = 9.0f;
	constexpr float HEXP_HEAT = 180.0f;
	constexpr float HEXP_MAX_TEMP = 520.0f + 273.15f;
	constexpr int HEXP_SHOK_COUNT = 4;
	constexpr int HEXP_RADIUS = 2;

	constexpr float BEXP_PRESSURE = 10.0f;
	constexpr float BEXP_PRESSURE_CAP = 12.0f;
	constexpr float BEXP_HEAT = 120.0f;
	constexpr int BEXP_RADIUS = 1;
	constexpr int BEXP_SHOK_COUNT = 3;

	constexpr float SEXP_PRESSURE = 3.0f;
	constexpr float SEXP_PRESSURE_CAP = 5.0f;
	constexpr float SEXP_HEAT = 80.0f;
	constexpr int SEXP_RADIUS = 2;
	constexpr int SEXP_DURATION = 12;
	constexpr int SEXP_SHOK_COUNT = 1;

	constexpr int SHOK_LIFE = 8;
	constexpr float SHOK_PRESSURE = 2.2f;
	constexpr float SHOK_PRESSURE_CAP = 3.0f;
	constexpr float SHOK_HEAT = 0.0f;
	constexpr int SHOK_RADIUS = 1;
	constexpr float SHOK_DECAY = 0.70f;

	constexpr float DENS_HEAT_RESISTANCE = 1900.0f;
	constexpr float DENS_PRESSURE_RESISTANCE = 16.0f;

	constexpr float PABS_PRESSURE_DAMPING = 0.72f;
	constexpr float PABS_HEAT_RESISTANCE = 1200.0f;

	constexpr float PWAL_PRESSURE_RESISTANCE = 22.0f;
	constexpr float PWAL_HEAT_RESISTANCE = 2300.0f;

	constexpr float PDRV_PRESSURE = 4.0f;
	constexpr float PDRV_PRESSURE_CAP = 6.0f;
	constexpr int PDRV_DIRECTION_MODE = 0;
	constexpr int PDRV_SHOK_COUNT = 2;
	constexpr int PDRV_LIFE = 16;

	struct FrameStats
	{
		uint64_t frame = 0;
		int fissionEvents = 0;
		int fissNeutronEmissions = 0;
		int ntrnCreated = 0;
		int shokCreated = 0;
		int compressedFissCount = 0;
		int activeFissCount = 0;
	};

	FrameStats GetFrameStats(Simulation *sim);
	void NoteFISSState(Simulation *sim, int compression, int activation);
	int ClampCompression(int value);
	int ClampActivation(int value);
	int CountNearbyType(Simulation *sim, int x, int y, int radius, int type);
	void AddPressureInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap);
	void AddHeatInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap);
	bool TryCreateNTRN(Simulation *sim, int x, int y, int radius);
	bool TryCreateSHOK(Simulation *sim, int x, int y, int radius);
	void ApplyFISSChainKick(Simulation *sim, int x, int y);
	void TryEmitCompressionNeutrons(Simulation *sim, int i, int x, int y, Parts &parts);
	bool PerformFissionEvent(Simulation *sim, int i, int x, int y, Parts &parts);
}
