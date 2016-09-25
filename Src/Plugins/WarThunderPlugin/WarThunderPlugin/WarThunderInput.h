//
// Get the warthunder motion data from http protocol
//
#pragma once

#include <thread>

//
// WarThunder Motion Input Variable
//
// http://localhost:8111/state
// {"valid": true,
// "aileron, %" : 46,
// "elevator, %" : 67,
// "rudder, %" : 73,
// "flaps, %" : 0,
// "gear, %" : 0,
// "TAS, km/h" : 0,
// "IAS, km/h" : 0,
// "M" : 0.00,
// "AoA, deg" : -0.0,
// "AoS, deg" : 0.0,
// "Ny" : 0.87,
// "Vy, m/s" : -0.0,
// "Wx, deg/s" : -0,
// "throttle 1, %" : 0,
// "RPM throttle 1, %" : 0,
// "mixture 1, %" : 100,
// "radiator 1, %" : 0,
// "compressor stage 1" : 2,
// "magneto 1" : 0,
// "power 1, hp" : -0.0,
// "RPM 1" : 0,
// "manifold pressure 1, atm" : 0.92,
// "oil temp 1, C" : 6,
// "pitch 1, deg" : 15.0,
// "thrust 1, kgs" : -0,
// "efficiency 1, %" : 0}
// 
// 
//http://localhost:8111/indicators
// {"valid": true,
// "type" : "spitfire_mk5c",
// "speed" : 179.051392,
// "pedals1" : 0.288732,
// "pedals2" : 0.288732,
// "pedals3" : 0.288732,
// "stick_elevator" : 0.355449,
// "stick_ailerons" : 0.000000,
// "vario" : -115.641251,
// "altitude_hour" : 2194.174072,
// "altitude_min" : 194.174072,
// "altitude_10k" : 2194.174072,
// "aviahorizon_roll" : -64.552498,
// "aviahorizon_pitch" : 38.228390,
// "bank" : 7.643487,
// "turn" : 0.439047,
// "compass" : 287.366730,
// "compass1" : 287.366730,
// "clock_hour" : 11.516666,
// "clock_min" : 31.000000,
// "clock_sec" : 29.000000,
// "manifold_pressure" : 2.262662,
// "rpm" : 3001.967285,
// "oil_pressure" : 92.743622,
// "oil_temperature" : 92.743622,
// "water_temperature" : 120.618378,
// "mixture" : 1.000000,
// "fuel1" : 287.810364,
// "gears" : 0.000000,
// "gears_lamp" : 1.000000,
// "flaps" : 0.000000,
// "trimmer" : 0.016911,
// "throttle" : 1.100000,
// "weapon1" : 0.000000,
// "weapon2" : 0.000000,
// "prop_pitch" : 1.000000}
//
//



class cWarThunderInput : public motion::cInput
{
public:
	cWarThunderInput();
	virtual ~cWarThunderInput();

	bool Init(const string *init, const string *math, const string *modulation);
	virtual bool Start() override;
	virtual bool Stop() override;
	virtual bool Update(const float deltaSeconds) override;


protected:
	void GetMotion(OUT float &roll, OUT float &pitch, OUT float &yaw);
	void InitSymbols();
	void UpdateSymbols();


public:
	enum MOTION_DATA { STATE, INDICATORS, ALL };

	// stats
	struct sState
	{
		float valid;
		float aileron;
		float elevator;
		float rudder;
		float flaps;
		float gear;
		float TAS;
		float IAS;
		float M;
		float AoA;
		float AoS;
		float Ny;
		float Vy;
		float Wx;
		float throttle;
		float RPM_throttle;
		float mixture;
		float radiator;
		float compressor_stage;
		float magneto;
		float power;
		float RPM;
		float manifold_pressure;
		float oil_temp;
		float pitch;
		float thrust;
		float efficiency;
	};

	//indicators
	struct sIndicators
	{
		float valid;
		float speed;
		float pedals1;
		float pedals2;
		float pedals3;
		float stick_elevator;
		float stick_ailerons;
		float vario;
		float altitude_hour;
		float altitude_min;
		float altitude_10k;
		float aviahorizon_roll;
		float aviahorizon_pitch;
		float bank;
		float turn;
		float compass;
		float compass1;
		float clock_hour;
		float clock_min;
		float clock_sec;
		float manifold_pressure;
		float rpm;
		float oil_pressure;
		float oil_temperature;
		float water_temperature;
		float mixture;
		float fuel1;
		float gears;
		float gears_lamp;
		float flaps;
		float trimmer;
		float throttle;
		float weapon1;
		float weapon2;
		float prop_pitch;
	};

	MOTION_DATA m_inputData;
	sState m_states;
	sIndicators m_indicators;
	float m_Roll;
	float m_Pitch;
	float m_Yaw;
	float m_Bank;

	cMathParser m_initParser;
	cMathParser m_mathParser;
	mathscript::cMathInterpreter m_interpreter;
	cMotionWaveModulator m_modulator;


	// thread variable
	std::thread m_thread;
	bool m_loop;
	int m_sleepMillis;
	common::CriticalSection m_critcalSection;
};

