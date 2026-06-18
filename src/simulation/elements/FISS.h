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
	constexpr int CFIS_MAX_CLUSTER_FISSIONS_PER_FRAME = 96;
	constexpr int MAX_NTRN_CREATED_PER_FRAME = 1400;
	constexpr int MAX_SHOK_CREATED_PER_FRAME = 220;
	constexpr float MAX_LOCAL_PRESSURE_FROM_FISS = 28.0f;
	constexpr float MAX_LOCAL_HEAT_FROM_FISS = 4800.0f;

	constexpr float CFIS_FORM_PRESSURE_THRESHOLD = 15.0f;
	constexpr int CFIS_FORM_COMPRESSION_THRESHOLD = 55;
	constexpr int CFIS_MIN_NEIGHBOURS = 5;
	constexpr int CFIS_FORM_RADIUS = 2;
	constexpr int CFIS_ARMING_DELAY = 60;
	constexpr int CFIS_COUNTDOWN_MIN = 90;
	constexpr int CFIS_COUNTDOWN_MAX = 120;
	constexpr int CFIS_DEFAULT_COUNTDOWN = 105;
	constexpr int CFIS_MIN_LIFE_BEFORE_FISSION = 90;
	constexpr int CFIS_PREFISSION_PHASE_FRAMES = 30;
	constexpr int CFIS_XRAY_START_AFTER_FORMATION = 10;
	constexpr int CFIS_HRAY_START_BEFORE_FISSION = 20;
	constexpr int CFIS_REVERT_GRACE_FRAMES = CFIS_XRAY_START_AFTER_FORMATION;
	constexpr int CFIS_SYNC_RADIUS = 2;
	constexpr int CFIS_SYNC_STRENGTH = 1;
	constexpr int CFIS_MAX_SYNC_REDUCTION_PER_FRAME = 1;
	constexpr int CFIS_SYNC_MIN_COUNTDOWN = 60;
	constexpr int CFIS_COMPRESSION_DECAY = 3;
	constexpr int CFIS_ACTIVATION_DECAY = 1;
	constexpr float CFIS_HOLD_STRENGTH = 0.6f;
	constexpr int CFIS_REVERT_TO_FISS_THRESHOLD = 22;
	constexpr bool CFIS_ZERO_VELOCITY_ON_FORM = true;
	constexpr bool CFIS_DAMP_VELOCITY_DURING_COUNTDOWN = true;
	constexpr float CFIS_VELOCITY_DAMPING_FACTOR = 0.20f;
	constexpr bool CFIS_RESIST_PRESSURE_SCATTER = true;
	constexpr int CFIS_FORM_COOLDOWN = 18;
	constexpr bool CFIS_REVERT_ON_LOW_NEIGHBOURS = true;
	constexpr int CFIS_MIN_NEIGHBOURS_TO_REMAIN = 3;
	constexpr int CFIS_PREFISSION_XRAY_CHANCE = 3600;
	constexpr int CFIS_PREFISSION_XRAY_MIN = 1;
	constexpr int CFIS_PREFISSION_XRAY_MAX = 3;
	constexpr int CFIS_PREFISSION_HRAY_CHANCE = 1800;
	constexpr int CFIS_PREFISSION_HRAY_MIN = 0;
	constexpr int CFIS_PREFISSION_HRAY_MAX = 2;
	constexpr int CFIS_PREFISSION_MAX_EMISSIONS_PER_FRAME = 220;
	constexpr bool CFIS_PREFISSION_REQUIRE_ARMED = false;

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

	constexpr float SEXP_PRESSURE = 1.5f;
	constexpr float SEXP_PRESSURE_CAP = BEXP_PRESSURE_CAP;
	constexpr float SEXP_HEAT = 16.0f;
	constexpr int SEXP_RADIUS = BEXP_RADIUS;
	constexpr int SEXP_DURATION = 8;
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

	constexpr bool THERMAL_FLASH_ENABLED = true;
	constexpr int THERMAL_FLASH_RADIUS = 20;
	constexpr float THERMAL_FLASH_INTENSITY = 360.0f;
	constexpr float THERMAL_FLASH_SURFACE_BONUS = 1.65f;
	constexpr float THERMAL_FLASH_DISTANCE_FALLOFF = 0.055f;
	constexpr float THERMAL_FLASH_MAX_HEAT = 720.0f;
	constexpr int THERMAL_FLASH_LINE_OF_SIGHT_STEPS = 24;
	constexpr int MAX_THERMAL_FLASH_EVENTS_PER_FRAME = 80;

	constexpr float HRAY_STAGE4_TEMP = MAX_TEMP;
	constexpr int HRAY_LIFE_MIN = 14;
	constexpr int HRAY_LIFE_MAX = 28;
	constexpr int HRAY_SPEED_MIN = 8;
	constexpr int HRAY_SPEED_MAX = 15;
	constexpr float HRAY_HEAT_DEPOSIT = 260.0f;
	constexpr float HRAY_SURFACE_HEAT_MULTIPLIER = 1.40f;
	constexpr int HRAY_ABSORPTION_CHANCE = 7200;
	constexpr int HRAY_PENETRATION_CHANCE = 1800;
	constexpr int MAX_HRAY_CREATED_PER_FRAME = 520;

	constexpr int XRAY_LIFE_MIN = 8;
	constexpr int XRAY_LIFE_MAX = 16;
	constexpr int XRAY_SPEED_MIN = 9;
	constexpr int XRAY_SPEED_MAX = 17;
	constexpr float XRAY_HEAT_DEPOSIT = 95.0f;
	constexpr int XRAY_ABSORPTION_CHANCE = 4200;
	constexpr int XRAY_PENETRATION_CHANCE = 4200;
	constexpr int XRAY_FISS_ACTIVATION_BONUS = 4;
	constexpr int MAX_XRAY_CREATED_PER_FRAME = 420;

	constexpr float RADS_START_TEMP = 980.0f + 273.15f;
	constexpr float RADS_MIN_TEMP = R_TEMP + 273.15f;
	constexpr float RADS_AMBIENT_TARGET_TEMP = R_TEMP + 273.15f;
	constexpr int RADS_LIFE_MIN = 240;
	constexpr int RADS_LIFE_MAX = 10800;
	constexpr int RADS_START_LIFE = RADS_LIFE_MIN;
	constexpr float RADS_COOLING_RATE = 1.8f;
	constexpr int RADS_XRAY_EMISSION_CHANCE = 18;
	constexpr int RADS_NTRN_EMISSION_CHANCE = 2;
	constexpr int RADS_DECAY_TIME = 1;
	constexpr int RADS_FINAL_DECAY_ELEMENT = PT_BRCK;
	constexpr int MAX_RADS_CREATED_PER_FRAME = 220;
	constexpr int MAX_RADS_EMISSIONS_PER_FRAME = 140;

	constexpr float IONZ_START_TEMP = 420.0f + 273.15f;
	constexpr int IONZ_LIFE = 22;
	constexpr float IONZ_COOLING_RATE = 8.0f;
	constexpr int IONZ_FADE_RATE = 1;

	constexpr int RMTR_SAMPLE_RADIUS = 7;
	constexpr int RMTR_DECAY_RATE = 6;
	constexpr int RMTR_MAX_SIGNAL = 255;

	constexpr int FISSION_HRAY_CHANCE = 8500;
	constexpr int FISSION_HRAY_MIN = 8;
	constexpr int FISSION_HRAY_MAX = 22;
	constexpr int FISSION_HRAY_RADIUS = 3;
	constexpr int FISSION_XRAY_CHANCE = 6200;
	constexpr int FISSION_XRAY_MIN = 4;
	constexpr int FISSION_XRAY_MAX = 14;
	constexpr int FISSION_XRAY_RADIUS = 4;
	constexpr int FISSION_RADS_CHANCE = 5000;
	constexpr int FISSION_RADS_MIN = 1;
	constexpr int FISSION_RADS_MAX = 5;

	constexpr int FPRD_XRAY_EMISSION_CHANCE = 14;
	constexpr int FPRD_RADS_DECAY_CHANCE = 1800;
	constexpr int FPRD_TO_RADS_CHANCE = 2400;
	constexpr int FPRD_TO_ASH_TIME = 1;

	constexpr int NABS_ABSORPTION_CHANCE = 7600;
	constexpr float NABS_HEAT_ON_ABSORB = 85.0f;
	constexpr int NABS_DAMAGE_PER_ABSORB = 8;
	constexpr int NABS_SPENT_THRESHOLD = 240;
	constexpr int NABS_SPENT_ELEMENT = PT_BRCK;

	constexpr int NSLW_SLOW_CHANCE = 7000;
	constexpr float NSLW_SPEED_MULTIPLIER = 0.45f;
	constexpr int NSLW_LIFE_CHANGE = -8;
	constexpr float NSLW_HEAT_ON_INTERACTION = 18.0f;

	constexpr int NDIFF_SCATTER_CHANCE = 6800;
	constexpr float NDIFF_SPEED_MULTIPLIER = 0.78f;
	constexpr int NDIFF_LIFE_PENALTY = 5;
	constexpr int NDIFF_ABSORPTION_CHANCE = 850;

	constexpr int RABS_XRAY_ABSORPTION_CHANCE = 7800;
	constexpr int RABS_HRAY_ABSORPTION_CHANCE = 8200;
	constexpr float RABS_HEAT_ON_XRAY_ABSORB = 140.0f;
	constexpr float RABS_HEAT_ON_HRAY_ABSORB = 320.0f;
	constexpr float RABS_OVERLOAD_TEMP = 1450.0f + 273.15f;

	constexpr bool RWAL_THERMAL_FLASH_BLOCKING = true;
	constexpr int RWAL_XRAY_ABSORPTION_CHANCE = 9000;
	constexpr int RWAL_HRAY_ABSORPTION_CHANCE = 9400;
	constexpr float RWAL_HEAT_ON_ABSORB = 230.0f;
	constexpr float RWAL_FAILURE_TEMP = 2400.0f + 273.15f;

	constexpr float HSINK_HEAT_ABSORPTION_RATE = 16.0f;
	constexpr int HSINK_SAMPLE_RADIUS = 2;
	constexpr float HSINK_MAX_SAFE_TEMP = 1800.0f + 273.15f;
	constexpr float HSINK_FAILURE_TEMP = 2600.0f + 273.15f;

	constexpr float PDMP_PRESSURE_DAMPING_RATE = 0.72f;
	constexpr int PDMP_PRESSURE_SAMPLE_RADIUS = 1;
	constexpr int PDMP_DAMAGE_PER_PRESSURE = 2;
	constexpr int PDMP_FAILURE_THRESHOLD = 220;

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
	int CountNearbyFissCluster(Simulation *sim, int x, int y, int radius);
	void AddPressureInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap);
	void AddHeatInRadius(Simulation *sim, int x, int y, int radius, float amount, float cap);
	bool TryCreateNTRN(Simulation *sim, int x, int y, int radius);
	bool TryCreateSHOK(Simulation *sim, int x, int y, int radius);
	bool TryCreateHRAY(Simulation *sim, int x, int y, int radius);
	bool TryCreateXRAY(Simulation *sim, int x, int y, int radius);
	bool TryCreateRADS(Simulation *sim, int x, int y, int radius);
	bool TryUseRadsEmissionSlot(Simulation *sim);
	bool TryAbsorbRadiantParticle(Simulation *sim, int particleIndex, int absorberIndex, int absorberType, bool heatRay);
	bool IsThermalFlashBlocker(int type);
	int RandomRadsLife(Simulation *sim);
	void ApplyThermalFlash(Simulation *sim, int x, int y, int radius, float intensity);
	void ApplyFISSChainKick(Simulation *sim, int x, int y);
	void TryEmitCompressionNeutrons(Simulation *sim, int i, int x, int y, Parts &parts);
	bool TryUseCFISClusterFissionSlot(Simulation *sim);
	bool TryUseCFISPrefissionEmissionSlot(Simulation *sim);
	bool PerformFissionEvent(Simulation *sim, int i, int x, int y, Parts &parts);
}
