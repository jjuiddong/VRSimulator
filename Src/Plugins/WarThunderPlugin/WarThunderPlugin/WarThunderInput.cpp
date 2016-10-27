
#include "stdafx.h"
#include "WarThunderInput.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>


using namespace motion;

unsigned WINAPI WarThunderThreadFunc(void* arg);


cWarThunderInput::cWarThunderInput() 
	: motion::cInput(MOTION_MEDIA::TCP)
	, m_loop(true)
	, m_sleepMillis(1)
	, m_inputData(ALL)
{
}

cWarThunderInput::~cWarThunderInput()
{
}


bool cWarThunderInput::Init(const string *init, const string *math, const string *modulation)
{
	if (init)
	{
		if (!m_initParser.ParseStr(*init))
			return false;
	}

	if (math)
	{
		if (!m_mathParser.ParseStr(*math))
			return false;
	}

	if (modulation)
		m_modulator.ParseStr(*modulation);

	return true;
}


bool cWarThunderInput::Start()
{
	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	InitSymbols();

	// :init 스크립트는 시작될 때, 한번 만 실행된다.
	m_interpreter.Excute(m_initParser.m_stmt);

	m_loop = true;
	m_thread = std::thread(WarThunderThreadFunc, this);

	m_state = MODULE_STATE::START;
	return true;
}


bool cWarThunderInput::Stop()
{
	m_state = MODULE_STATE::STOP;

	if (m_thread.joinable())
	{
		m_loop = false;
		m_thread.join();
	}

	return true;
}


bool cWarThunderInput::Update(const float deltaSeconds)
{
	using namespace script;

	RETV(m_state == MODULE_STATE::STOP, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	UpdateSymbols();

	// Excute Math Script
	m_interpreter.Excute(m_mathParser.m_stmt);
	
	// Calc Modulation
	const float roll = script::g_symbols["@roll"].fVal;
	const float pitch = script::g_symbols["@pitch"].fVal;
	const float yaw = script::g_symbols["@yaw"].fVal;
	const float heave = script::g_symbols["@heave"].fVal;
	m_modulator.Update(m_incTime, yaw, pitch, roll, heave);

	// Save Symboltable Modulation Output Data
	float froll, fpitch, fyaw, fheave;
	m_modulator.GetFinal(fyaw, fpitch, froll, fheave);

	script::sFieldData data;
	data.type = script::FIELD_TYPE::T_FLOAT;
	data.fVal = froll;
	script::g_symbols["@roll_mod"] = data;
	data.fVal = fpitch;
	script::g_symbols["@pitch_mod"] = data;
	data.fVal = fyaw;
	script::g_symbols["@yaw_mod"] = data;
	data.fVal = fheave;
	script::g_symbols["@heave_mod"] = data;

	m_incTime = 0;
	return true;
}


void cWarThunderInput::InitSymbols()
{
	script::sFieldData data;
	data.type = script::FIELD_TYPE::T_FLOAT;
	data.fVal = 0;
	string symbols[] = {

		// state
		"@state_valid",
		"@state_aileron",
		"@state_elevator",
		"@state_rudder",
		"@state_flaps",
		"@state_gear",
		"@state_TAS",
		"@state_IAS",
		"@state_M",
		"@state_AoA",
		"@state_AoS",
		"@state_Ny",
		"@state_Vy",
		"@state_Wx",
		"@state_throttle",
		"@state_RPM_throttle",
		"@state_mixture",
		"@state_radiator",
		"@state_compressor_stage",
		"@state_magneto",
		"@state_power",
		"@state_RPM",
		"@state_manifold_pressure",
		"@state_oil_temp",
		"@state_pitch",
		"@state_thrust",
		"@state_efficiency",

		// indicator
		"@indicator_valid",
		"@indicator_speed",
		"@indicator_pedals1",
		"@indicator_pedals2",
		"@indicator_pedals3",
		"@indicator_stick_elevator",
		"@indicator_stick_ailerons",
		"@indicator_vario",
		"@indicator_altitude_hour",
		"@indicator_altitude_min",
		"@indicator_altitude_10k",
		"@indicator_aviahorizon_roll",
		"@indicator_aviahorizon_pitch",
		"@indicator_bank",
		"@indicator_turn",
		"@indicator_compass",
		"@indicator_compass1",
		"@indicator_clock_hour",
		"@indicator_clock_min",
		"@indicator_clock_sec",
		"@indicator_manifold_pressure",
		"@indicator_rpm",
		"@indicator_oil_pressure",
		"@indicator_oil_temperature",
		"@indicator_water_temperature",
		"@indicator_mixture",
		"@indicator_fuel1",
		"@indicator_gears",
		"@indicator_gears_lamp",
		"@indicator_flaps",
		"@indicator_trimmer",
		"@indicator_throttle",
		"@indicator_weapon1",
		"@indicator_weapon2",
		"@indicator_prop_pitch",
	};
	
	AutoCSLock cs(m_critcalSection);
	const int len = sizeof(symbols) / sizeof(string);
	for (int i=0; i < len; ++i)
		script::g_symbols[ symbols[i]] = data;
}


void cWarThunderInput::UpdateSymbols()
{
	AutoCSLock cs(m_critcalSection);

	// state
	script::g_symbols["@state_valid"].fVal = m_states.valid;
	script::g_symbols["@state_aileron"].fVal = m_states.aileron;
	script::g_symbols["@state_elevator"].fVal = m_states.elevator;
	script::g_symbols["@state_rudder"].fVal = m_states.rudder;
	script::g_symbols["@state_flaps"].fVal = m_states.flaps;
	script::g_symbols["@state_gear"].fVal = m_states.gear;
	script::g_symbols["@state_TAS"].fVal = m_states.TAS;
	script::g_symbols["@state_IAS"].fVal = m_states.IAS;
	script::g_symbols["@state_M"].fVal = m_states.M;
	script::g_symbols["@state_AoA"].fVal = m_states.AoA;
	script::g_symbols["@state_AoS"].fVal = m_states.AoS;
	script::g_symbols["@state_Ny"].fVal = m_states.Ny;
	script::g_symbols["@state_Vy"].fVal = m_states.Vy;
	script::g_symbols["@state_Wx"].fVal = m_states.Wx;
	script::g_symbols["@state_throttle"].fVal = m_states.throttle;
	script::g_symbols["@state_RPM_throttle"].fVal = m_states.RPM_throttle;
	script::g_symbols["@state_mixture"].fVal = m_states.mixture;
	script::g_symbols["@state_radiator"].fVal = m_states.radiator;
	script::g_symbols["@state_compressor_stage"].fVal = m_states.compressor_stage;
	script::g_symbols["@state_magneto"].fVal = m_states.magneto;
	script::g_symbols["@state_power"].fVal = m_states.power;
	script::g_symbols["@state_RPM"].fVal = m_states.RPM;
	script::g_symbols["@state_manifold_pressure"].fVal = m_states.manifold_pressure;
	script::g_symbols["@state_oil_temp"].fVal = m_states.oil_temp;
	script::g_symbols["@state_pitch"].fVal = m_states.pitch;
	script::g_symbols["@state_thrust"].fVal = m_states.thrust;
	script::g_symbols["@state_efficiency"].fVal = m_states.efficiency;

	// indicator
	script::g_symbols["@indicator_valid"].fVal = m_indicators.valid;
	script::g_symbols["@indicator_speed"].fVal = m_indicators.speed;
	script::g_symbols["@indicator_pedals1"].fVal = m_indicators.pedals1;
	script::g_symbols["@indicator_pedals2"].fVal = m_indicators.pedals2;
	script::g_symbols["@indicator_pedals3"].fVal = m_indicators.pedals3;
	script::g_symbols["@indicator_stick_elevator"].fVal = m_indicators.stick_elevator;
	script::g_symbols["@indicator_stick_ailerons"].fVal = m_indicators.stick_ailerons;
	script::g_symbols["@indicator_vario"].fVal = m_indicators.vario;
	script::g_symbols["@indicator_altitude_hour"].fVal = m_indicators.altitude_hour;
	script::g_symbols["@indicator_altitude_min"].fVal = m_indicators.altitude_min;
	script::g_symbols["@indicator_altitude_10k"].fVal = m_indicators.altitude_10k;
	script::g_symbols["@indicator_aviahorizon_roll"].fVal = m_indicators.aviahorizon_roll;
	script::g_symbols["@indicator_aviahorizon_pitch"].fVal = m_indicators.aviahorizon_pitch;
	script::g_symbols["@indicator_bank"].fVal = m_indicators.bank;
	script::g_symbols["@indicator_turn"].fVal = m_indicators.turn;
	script::g_symbols["@indicator_compass"].fVal = m_indicators.compass;
	script::g_symbols["@indicator_compass1"].fVal = m_indicators.compass1;
	script::g_symbols["@indicator_clock_hour"].fVal = m_indicators.clock_hour;
	script::g_symbols["@indicator_clock_min"].fVal = m_indicators.clock_min;
	script::g_symbols["@indicator_clock_sec"].fVal = m_indicators.clock_sec;
	script::g_symbols["@indicator_manifold_pressure"].fVal = m_indicators.manifold_pressure;
	script::g_symbols["@indicator_rpm"].fVal = m_indicators.rpm;
	script::g_symbols["@indicator_oil_pressure"].fVal = m_indicators.oil_pressure;
	script::g_symbols["@indicator_oil_temperature"].fVal = m_indicators.oil_temperature;
	script::g_symbols["@indicator_water_temperature"].fVal = m_indicators.water_temperature;
	script::g_symbols["@indicator_mixture"].fVal = m_indicators.mixture;
	script::g_symbols["@indicator_fuel1"].fVal = m_indicators.fuel1;
	script::g_symbols["@indicator_gears"].fVal = m_indicators.gears;
	script::g_symbols["@indicator_gears_lamp"].fVal = m_indicators.gears_lamp;
	script::g_symbols["@indicator_flaps"].fVal = m_indicators.flaps;
	script::g_symbols["@indicator_trimmer"].fVal = m_indicators.trimmer;
	script::g_symbols["@indicator_throttle"].fVal = m_indicators.throttle;
	script::g_symbols["@indicator_weapon1"].fVal = m_indicators.weapon1;
	script::g_symbols["@indicator_weapon2"].fVal = m_indicators.weapon2;
	script::g_symbols["@indicator_prop_pitch"].fVal = m_indicators.prop_pitch;
}


void cWarThunderInput::GetMotion(OUT float &roll, OUT float &pitch, OUT float &yaw)
{
	AutoCSLock cs(m_critcalSection);
	roll = m_Roll;
	pitch = m_Pitch;
	yaw = m_Yaw;
}



// 주기적으로, War Thunder Motion 정보를 가져온다.
unsigned WINAPI WarThunderThreadFunc(void* arg)
{
	cWarThunderInput *wtInput = (cWarThunderInput*)arg;
	network::cTCPClient client;
	client.m_isLog = false;

	cWarThunderInput::MOTION_DATA readType = cWarThunderInput::STATE;
	string cmd[] = {
		"GET /state HTTP/1.1\r\n \r\nConnection: close\r\n\r\n",
		"GET /indicators HTTP/1.1\r\n \r\nConnection: close\r\n\r\n"
	};

	while (wtInput->m_loop)
	{
		if (!client.Init("localhost", 8111, 512, 10, 30, true))
		{
			Sleep(100);
			continue;
		}

		if ((wtInput->m_inputData != readType) && (wtInput->m_inputData != cWarThunderInput::ALL))
			readType = (readType == cWarThunderInput::STATE) ? cWarThunderInput::INDICATORS : cWarThunderInput::STATE;

		const string str = cmd[readType];
		client.Send((BYTE*)str.c_str(), str.length());

		// Receive Motion Data from server
		stringstream ss;
		while (client.IsConnect())
		{
			network::sSockBuffer packet;
			while (client.m_recvQueue.Front(packet))
			{
				packet.buffer[packet.actualLen] = NULL;
				ss << packet.buffer;
				client.m_recvQueue.Pop();
			}
		}

		// json Parsing
		try
		{
			using boost::property_tree::ptree;
			using std::string;
			ptree props;

			const string str = ss.str();
			const char *next = strchr(str.c_str(), '{');
			if (next)
			{
				std::istringstream iss(next);
				boost::property_tree::read_json(iss, props);

				switch (readType)
				{
				case cWarThunderInput::STATE:
				{
					wtInput->m_critcalSection.Lock();
					wtInput->m_states.valid = (float)(props.get<string>("valid", "false") == "true");
					wtInput->m_states.aileron = (float)atof(props.get<string>("aileron, %", "").c_str());
					wtInput->m_states.elevator = (float)atof(props.get<string>("elevator, %", "").c_str());
					wtInput->m_states.rudder = (float)atof(props.get<string>("rudder, %", "").c_str());
					wtInput->m_states.flaps = (float)atof(props.get<string>("flaps, %", "").c_str());
					wtInput->m_states.gear = (float)atof(props.get<string>("gear, %", "").c_str());
					wtInput->m_states.TAS = (float)atof(props.get<string>("TAS, km/h", "").c_str());
					wtInput->m_states.IAS = (float)atof(props.get<string>("IAS, km/h", "").c_str());
					wtInput->m_states.M = (float)atof(props.get<string>("M", "").c_str());
					wtInput->m_states.AoA = (float)atof(props.get<string>("AoA, deg", "").c_str());
					wtInput->m_states.AoS = (float)atof(props.get<string>("AoS, deg", "").c_str());
					wtInput->m_states.Ny = (float)atof(props.get<string>("Ny", "").c_str());
					wtInput->m_states.Vy = (float)atof(props.get<string>("Vy, m/s", "").c_str());
					wtInput->m_states.Wx = (float)atof(props.get<string>("Wx, deg/s", "").c_str());
					wtInput->m_states.throttle = (float)atof(props.get<string>("throttle 1, %", "").c_str());
					wtInput->m_states.RPM_throttle = (float)atof(props.get<string>("RPM throttle 1, %", "").c_str());
					wtInput->m_states.mixture = (float)atof(props.get<string>("mixture 1, %", "").c_str());
					wtInput->m_states.radiator = (float)atof(props.get<string>("radiator 1, %", "").c_str());
					wtInput->m_states.compressor_stage = (float)atof(props.get<string>("compressor stage 1", "").c_str());
					wtInput->m_states.magneto = (float)atof(props.get<string>("magneto 1", "").c_str());
					wtInput->m_states.power = (float)atof(props.get<string>("power 1, hp", "").c_str());
					wtInput->m_states.RPM = (float)atof(props.get<string>("RPM 1", "").c_str());
					wtInput->m_states.manifold_pressure = (float)atof(props.get<string>("manifold pressure 1, atm", "").c_str());
					wtInput->m_states.oil_temp = (float)atof(props.get<string>("oil temp 1, C", "").c_str());
					wtInput->m_states.pitch = (float)atof(props.get<string>("pitch 1, deg", "").c_str());
					wtInput->m_states.thrust = (float)atof(props.get<string>("thrust 1, kgs", "").c_str());
					wtInput->m_states.efficiency = (float)atof(props.get<string>("efficiency 1, %", "").c_str());
					wtInput->m_critcalSection.Unlock();
				}
				break;

				case cWarThunderInput::INDICATORS:
				{
 					wtInput->m_critcalSection.Lock();
					wtInput->m_indicators.valid = (float)(props.get<string>("valid", "false") == "true");
					wtInput->m_indicators.speed = (float)atof(props.get<string>("speed", "").c_str());
					wtInput->m_indicators.pedals1 = (float)atof(props.get<string>("pedals1", "").c_str());
					wtInput->m_indicators.pedals2 = (float)atof(props.get<string>("pedals2", "").c_str());
					wtInput->m_indicators.pedals3 = (float)atof(props.get<string>("pedals3", "").c_str());
					wtInput->m_indicators.stick_elevator = (float)atof(props.get<string>("stick_elevator", "").c_str());
					wtInput->m_indicators.stick_ailerons = (float)atof(props.get<string>("stick_ailerons", "").c_str());
					wtInput->m_indicators.vario = (float)atof(props.get<string>("vario", "").c_str());
					wtInput->m_indicators.altitude_hour = (float)atof(props.get<string>("altitude_hour", "").c_str());
					wtInput->m_indicators.altitude_min = (float)atof(props.get<string>("altitude_min", "").c_str());
					wtInput->m_indicators.altitude_10k = (float)atof(props.get<string>("altitude_10k", "").c_str());
					wtInput->m_indicators.aviahorizon_roll = (float)atof(props.get<string>("aviahorizon_roll", "").c_str());
					wtInput->m_indicators.aviahorizon_pitch = (float)atof(props.get<string>("aviahorizon_pitch", "").c_str());
					wtInput->m_indicators.bank = (float)atof(props.get<string>("bank", "").c_str());
					wtInput->m_indicators.turn = (float)atof(props.get<string>("turn", "").c_str());
					wtInput->m_indicators.compass = (float)atof(props.get<string>("compass", "").c_str());
					wtInput->m_indicators.compass1 = (float)atof(props.get<string>("compass1", "").c_str());
					wtInput->m_indicators.clock_hour = (float)atof(props.get<string>("clock_hour", "").c_str());
					wtInput->m_indicators.clock_min = (float)atof(props.get<string>("clock_min", "").c_str());
					wtInput->m_indicators.clock_sec = (float)atof(props.get<string>("clock_sec", "").c_str());
					wtInput->m_indicators.manifold_pressure = (float)atof(props.get<string>("manifold_pressure", "").c_str());
					wtInput->m_indicators.rpm = (float)atof(props.get<string>("rpm", "").c_str());
					wtInput->m_indicators.oil_pressure = (float)atof(props.get<string>("oil_pressure", "").c_str());
					wtInput->m_indicators.oil_temperature = (float)atof(props.get<string>("oil_temperature", "").c_str());
					wtInput->m_indicators.water_temperature = (float)atof(props.get<string>("water_temperature", "").c_str());
					wtInput->m_indicators.mixture = (float)atof(props.get<string>("mixture", "").c_str());
					wtInput->m_indicators.fuel1 = (float)atof(props.get<string>("fuel1", "").c_str());
					wtInput->m_indicators.gears = (float)atof(props.get<string>("gears", "").c_str());
					wtInput->m_indicators.gears_lamp = (float)atof(props.get<string>("gears_lamp", "").c_str());
					wtInput->m_indicators.flaps = (float)atof(props.get<string>("flaps", "").c_str());
					wtInput->m_indicators.trimmer = (float)atof(props.get<string>("trimmer", "").c_str());
					wtInput->m_indicators.throttle = (float)atof(props.get<string>("throttle", "").c_str());
					wtInput->m_indicators.weapon1 = (float)atof(props.get<string>("weapon1", "").c_str());
					wtInput->m_indicators.weapon2 = (float)atof(props.get<string>("weapon2", "").c_str());
					wtInput->m_indicators.prop_pitch = (float)atof(props.get<string>("prop_pitch", "").c_str());
					wtInput->m_critcalSection.Unlock();
				}
				break;
				}
			}
		}
		catch (...)
		{
			// nothing~
		}

		// next read motion data
		readType = (readType == cWarThunderInput::STATE) ? cWarThunderInput::INDICATORS : cWarThunderInput::STATE;

		Sleep(wtInput->m_sleepMillis);
	}

	return 0;
}

