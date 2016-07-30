//
// 2016-07-13, jjuiddong
// Dirt3 모션 처리
//
#pragma once

class cStrikerX
{
public:
	enum STATE {
		OFF,
		SERVOON,
		READY,
		START,
		PLAY,
		TOREADY,
		STOP,
		SERVOOFF,
		DELAY,
	};

	enum MOTION_PROTOCOL {
		PRT_MOTION,
		PRT_SERVOON,
		PRT_SERVOOFF,
		PRT_START,
		PRT_STOP,
	};

	cStrikerX();
	virtual ~cStrikerX();

	int Init(HWND hWnd);
	int UpdateMotionScript();
	int Update(const float deltaSeconds);
	void End();
	void Clear();


protected:
	void Delay(const float delaySeconds, STATE nextState);
	void SendSerialPort();
	void CheckAutoStart(const float deltaSeconds);
	void CheckAutoStop(const float deltaSeconds);


protected:
	STATE m_state;
	HWND m_hWnd;

	// delay
	STATE m_nextState;
	float m_delaySeconds;

	// labTime
	float m_lastLabTime;
	int m_lapTimeUpCount;
	int m_sameLapTimeCount;

	// strikerx
	bool m_oldGameState;
	float m_oldRoll;
	float m_oldPitch;
	float m_oldYaw;
	float m_oldHeave;
	float m_oldTime;
	int m_sameDataCount;
	int m_differentDataCount;
	float m_checkDiffTime;
};

