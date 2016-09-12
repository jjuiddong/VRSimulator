//
// 2016-06-30, jjuiddong
// War Thunder 모션정보를 가져온다.
//
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
#pragma once

namespace motion
{
	class cWarThunderMotionInput
	{
	public:
		cWarThunderMotionInput();
		virtual ~cWarThunderMotionInput();

		enum MOTION_DATA { STATE, INDICATORS, ALL };
		bool Init(const MOTION_DATA inputData = INDICATORS);
		void GetMotion(OUT float &roll, OUT float &pitch, OUT float &yaw);
		void Clear();


	public:
		MOTION_DATA m_inputData;
		float m_Roll;
		float m_Pitch;
		float m_Yaw;
		float m_Bank;

		// thread variable
		HANDLE m_handle;
		DWORD m_threadId;
		bool m_threadLoop;
		int m_sleepMillis;
		common::CriticalSection m_critcalSection;
	};
}
