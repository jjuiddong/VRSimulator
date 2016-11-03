
#include "stdafx.h"
#include "VRSim.h"
#include "VRSimDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif


CVRSimDlg::CVRSimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VRSIM_DIALOG, pParent)
	, m_isLoop(true)
	, m_state(DETECT_GAME)
	, m_delaySeconds(0)
	, m_isViewDetails(false)
	, m_editSpeed(0)
	, m_selectGame(-1)
	, m_strGameTitle(_T(""))
	, m_radioAxisType(0)
	, m_editLimitTime(_T("5.0"))
	, m_image(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVRSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FPS, m_staticFPS);
	DDX_Control(pDX, IDC_BUTTON_DETAILS, m_buttonDetails);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_sliderSpeed);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_editSpeed);
	DDX_Text(pDX, IDC_STATIC_TITLE, m_strGameTitle);
	DDX_Control(pDX, IDC_STATIC_PLAY_COUNT, m_staticPlayGames);
	DDX_Control(pDX, IDC_STATIC_TOTAL_PLAY_COUNT, m_staticTotalGames);
	DDX_Radio(pDX, IDC_RADIO_AXIS3, m_radioAxisType);
	DDX_Text(pDX, IDC_EDIT_LIMIT_MINUTE, m_editLimitTime);
}

BEGIN_MESSAGE_MAP(CVRSimDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CVRSimDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVRSimDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DETAILS, &CVRSimDlg::OnBnClickedButtonDetails)
	ON_BN_CLICKED(IDC_BUTTON_SERVOON, &CVRSimDlg::OnBnClickedButtonServoon)
	ON_BN_CLICKED(IDC_BUTTON_SERVOOFF, &CVRSimDlg::OnBnClickedButtonServooff)
	ON_BN_CLICKED(IDC_BUTTON_SERVOSTART, &CVRSimDlg::OnBnClickedButtonServostart)
	ON_BN_CLICKED(IDC_BUTTON_SERVOSTOP, &CVRSimDlg::OnBnClickedButtonServostop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SPEED, &CVRSimDlg::OnNMCustomdrawSliderSpeed)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CVRSimDlg::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_RADIO_AXIS3, &CVRSimDlg::OnBnClickedRadioAxis3)
	ON_BN_CLICKED(IDC_RADIO_AXIS4, &CVRSimDlg::OnBnClickedRadioAxis4)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT_LIMIT_MINUTE, &CVRSimDlg::OnEnChangeEditLimitMinute)
END_MESSAGE_MAP()


BOOL CVRSimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);		// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize GDI Plus
	Gdiplus::GdiplusStartupInput k_Input;
	Gdiplus::GdiplusStartup(&m_gdiPlusToken, &k_Input, NULL);


	//UI ----------------------------------------------------------------
	// Hide Setting UI
	CRect btnWr;
	m_buttonDetails.GetWindowRect(btnWr);
	CRect wr;
	GetWindowRect(wr);
	wr.bottom = wr.top + btnWr.bottom + 10;
	MoveWindow(wr);
	m_sliderSpeed.SetRange(0, 1000);
	ReadConfigFile();
	//----------------------------------------------------------------

	dbg::RemoveLog();
	dbg::RemoveErrLog();

	// load default plugin for ServoOn, ServoOff, Start, Stop
	sPluginInfo defaultPlugin;
	if (defaultPlugin.Load("./Plugins/DefaultPlugin.dll"))
		m_plugins.push_back(defaultPlugin);

	list<string> exts;
	exts.push_back("dll");
	list<string> files;
	common::CollectFiles(exts, "./Plugins/", files);

	for each (auto file in files)
	{
		if (file.find("DefaultPlugin.dll") != string::npos)
			continue; // ignore default plugin, already load

		sPluginInfo plugin;
		if (plugin.Load(file))
		{
			for each (auto &gameName in plugin.gameNames)
				m_findProc.AddFindProcess(gameName);

			m_plugins.push_back(plugin);
		}
		else
		{
			::AfxMessageBox(formatw("Error!! Load Plugin [%s] errorCode=%d \n", file.c_str()).c_str(), GetLastError() );
			return FALSE;
		}
	}

	m_findProc.Init(); // start find process
	UpdatePlayInfo();
	SendServoMessage(SERVO_ON);

	return TRUE;
}


void CVRSimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CWnd *wnd = GetDlgItem(IDC_STATIC_IMAGE);
		if (wnd && m_image)
		{
			CPaintDC dc(this);
			Gdiplus::Graphics g(dc);
			CRect wr;
			wnd->GetWindowRect(wr);
			ScreenToClient(wr);
			g.DrawImage(m_image, Gdiplus::Rect(wr.left, wr.top, wr.Width(), wr.Height()));
		}

		CDialogEx::OnPaint();
	}
}


HCURSOR CVRSimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVRSimDlg::OnBnClickedOk()
{
}

void CVRSimDlg::OnBnClickedCancel()
{
	m_isLoop = false;
	m_state = EXIT;

	for each (auto plugin in m_plugins)
		FreeLibrary(plugin.lib);
	m_plugins.clear();

	CDialogEx::OnCancel();
}


// Main Loop Function
bool CVRSimDlg::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	int fps = 0;
	int lastFpsTime = timeGetTime();
	int oldT = timeGetTime();
	while (m_isLoop)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const int curT = timeGetTime();
		const float deltaSeconds = (float)(curT - oldT) * 0.001f;
		if (deltaSeconds > 0.02f) // 50 frames
		{
			oldT = curT;

			// Check FPS
			++fps;
			if (curT - lastFpsTime > 1000)
			{
				m_staticFPS.SetWindowText(common::formatw("FPS : %d", fps).c_str());
				fps = 0;
				lastFpsTime = curT;
			}

			MainLoop(deltaSeconds);
		}

		Sleep(1);
	}

	return true;
}


void CVRSimDlg::MainLoop(const float deltaSeconds)
{
	switch (m_state)
	{
	case DETECT_GAME: DetectGameLoop(deltaSeconds); break;
	case MOTION_LOOP: MotionLoop(deltaSeconds); break;
	case MOTION_END_LOOP: MotionEndLoop(deltaSeconds); break;
	case EXIT: break;
	default: assert(0);break;
	}
}


// detect running game 
// if plugins game, start motion
void CVRSimDlg::DetectGameLoop(const float deltaSeconds)
{
	// check every 1 seconds
	static float incT = 0;
	incT += deltaSeconds;
	if (incT < 1)
		return;
	incT = 0;
	//

	for (u_int i=0; i < m_plugins.size(); ++i)
	{
		const int gameIdx = IsLiveGame(m_plugins[i]);
		if (gameIdx  >= 0)
		{
			UpdateData();

			// clear current plugin
			if (m_selectGame >= 0)
				m_plugins[m_selectGame].MotionClear();

			m_selectGame = i;
			m_state = MOTION_LOOP;
			m_strGameTitle = m_plugins[i].outputGameNames[gameIdx].c_str();
			m_image = new Gdiplus::Bitmap(m_plugins[i].gameLogoFiles[gameIdx].c_str());

			// detect game, initialize motion dll
			if (m_plugins[i].MotionInit((int)m_hWnd, gameIdx) <= 0)
			{
				::AfxMessageBox(L"Error Motion Init \n");
			}

			// Update Motion Script Global Variable
			m_plugins[i].MotionSetSymbol("@motion_speed", m_editSpeed);
			m_plugins[i].MotionSetSymbol("@yaw_speed", (float)m_radioAxisType);
			const float limitTime = (float)_ttof((LPCTSTR)m_editLimitTime);
			m_plugins[i].MotionSetSymbol("@limit_time", limitTime);
			//

			//SetBackgroundColor(g_blueColor);
			UpdateData(FALSE);
			InvalidateRect(NULL);
			break;
		}
	}
}

// every time, call this function
void CVRSimDlg::MotionLoop(const float deltaSeconds)
{
	if (m_plugins.empty() || (m_selectGame < 0))
	{
		goto not_detect_game;
	}

	static float incT = 0;
	incT += deltaSeconds;
	if (incT > 1.f) // check every 1 seconds
	{
		incT = 0;
		if (IsLiveGame(m_plugins[m_selectGame]) == -1)
		{
			goto not_detect_game;
		}
	}

	const int result = m_plugins[m_selectGame].MotionUpdate(deltaSeconds);
	if (result == 2) // game end, update game.csv
		UpdatePlayInfo();

	return;


not_detect_game:
	m_state = MOTION_END_LOOP;
	m_strGameTitle = L"";
	SAFE_DELETE(m_image);
	if (m_selectGame >= 0)
		m_plugins[m_selectGame].MotionEnd();
	//SetBackgroundColor(g_grayColor);
	UpdateData(FALSE);
	InvalidateRect(NULL);
}


void CVRSimDlg::MotionEndLoop(const float deltaSeconds)
{
	if (m_plugins.empty())
		return;
	if (m_selectGame < 0)
	{
		m_state = DETECT_GAME;
		return;
	}

	const int result = m_plugins[m_selectGame].MotionUpdate(deltaSeconds);
	if (result == 0)
	{ // motion device finish
		m_state = DETECT_GAME;
		m_plugins[m_selectGame].MotionClear();
		m_selectGame = -1;
		UpdateData(FALSE);
	}
}


// check running game
// return value = -1 : not find game
//							0 >= : find game, return game  index 
int CVRSimDlg::IsLiveGame(const sPluginInfo &plugin)
{
	const wstring runningProcess = m_findProc.GetRunningProcess();
	if (runningProcess.empty())
		return -1;

	for (u_int i = 0; i < plugin.gameNames.size(); ++i)
	{
		if (runningProcess == plugin.gameNames[i])
			return i;
	}

	return -1;
}


void CVRSimDlg::OnBnClickedButtonDetails()
{
	const float EXPAND_RATE = 2.f;

	if (m_isViewDetails)
	{
		CRect wr;
		GetWindowRect(wr);
		wr.bottom = wr.top + (int)(wr.Height() * 1.f / EXPAND_RATE);
		MoveWindow(wr);
		m_buttonDetails.SetWindowTextW(L"View Details >>");
	}
	else
	{
		CRect wr;
		GetWindowRect(wr);
		wr.bottom = wr.top + (int)(wr.Height() * EXPAND_RATE);
		MoveWindow(wr);
		m_buttonDetails.SetWindowTextW(L"<< View Details");
	}

	m_isViewDetails = !m_isViewDetails;
}


void CVRSimDlg::OnBnClickedButtonServoon()
{
	SendServoMessage(SERVO_ON);
}


void CVRSimDlg::OnBnClickedButtonServooff()
{
	SendServoMessage(SERVO_OFF);
}


void CVRSimDlg::OnBnClickedButtonServostart()
{
	SendServoMessage(SERVO_START);
}


void CVRSimDlg::OnBnClickedButtonServostop()
{
	SendServoMessage(SERVO_STOP);
	SendServoMessage(SERVO_ON);
}


static const string g_ConfigFileName = "./config.ini";
// config.ini 파일로 부터 axis, motion speed 값을 가져온다.
void CVRSimDlg::ReadConfigFile()
{
	const float motionSpeed = uiutil::GetProfileFloat("config", "motion_speed", 1.f, g_ConfigFileName);
	const float yawSpeed = uiutil::GetProfileFloat("config", "yaw_speed", 0.f, g_ConfigFileName);
	const float limitTime = uiutil::GetProfileFloat("config", "limit_time", 5.f, g_ConfigFileName);

 	m_radioAxisType = (yawSpeed > 0) ? 1 : 0;
 	m_editSpeed = motionSpeed;
 	m_sliderSpeed.SetPos((int)(motionSpeed * 1000.f));
	m_editLimitTime.Format(L"%.1f", limitTime);
	UpdateData(FALSE);
}


// config.ini 파일에 axis, motion speed 값을 저장한다.
void CVRSimDlg::WriteConfigFile()
{
	UpdateData();

	using namespace std;
	ofstream ofs(g_ConfigFileName);
	if (!ofs.is_open())
		return;

 	const float pos = (float)m_sliderSpeed.GetPos() / 1000.f;
	const float limitTime = (float)_ttof((LPCTSTR)m_editLimitTime);

 	ofs << "[config]" << endl;
 	ofs << "yaw_speed = " << m_radioAxisType << endl;
 	ofs << "motion_speed = " << pos << endl;
	ofs << "limit_time = " << limitTime << endl;
}


void CVRSimDlg::SendServoMessage(const int cmd)
{
	if (m_selectGame >= 1)
		m_plugins[m_selectGame].MotionClear();
	if (m_selectGame != 0)
		m_plugins[0].MotionInit((int)m_hWnd, 0); // initialize default plugin dll

	m_selectGame = 0; // set default plugin dll 
	m_plugins[0].MotionSetOutputFormat(cmd);
	for (int i = 0; i < 10; ++i)
	{
		m_plugins[0].MotionUpdate(50);
		Sleep(50);
	}
}


void CVRSimDlg::OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	const float pos = (float)m_sliderSpeed.GetPos() / 1000.f;
	m_editSpeed = pos;
	UpdateData(FALSE);

	if (m_selectGame >= 0)
		m_plugins[m_selectGame].MotionSetSymbol("@motion_speed", pos);

	*pResult = 0;
}


void CVRSimDlg::OnBnClickedButtonUpdate()
{
	if (m_selectGame >= 0)
	{
		UpdateData();

		m_plugins[m_selectGame].MotionUpdateScript();
		m_plugins[m_selectGame].MotionSetSymbol("@motion_speed", m_editSpeed);
		m_plugins[m_selectGame].MotionSetSymbol("@yaw_speed", (float)m_radioAxisType);
		const float limitTime = (float)_ttof((LPCTSTR)m_editLimitTime);
		m_plugins[m_selectGame].MotionSetSymbol("@limit_time", limitTime);
	}
}


// update today play game count
// parse game.csv to find today play games
// and total play game
void CVRSimDlg::UpdatePlayInfo()
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	ifstream ifs("game.csv");
	if (!ifs.is_open())
		return;

	string today;
	{
		time_t time = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &time);
		stringstream ss;
		ss << std::put_time(&tm, "%Y-%m-%d");
		today = ss.str();
	}

	int todayPlayGameCount = 0;
	int totalGames = 0;
	string line;
	while (getline(ifs, line))
	{
		stringstream ss(line);
		string dateStr;
		ss >> dateStr;

		++totalGames;
		if (dateStr == today)
			++todayPlayGameCount;
	}

	m_staticPlayGames.SetWindowText(common::formatw("Game Count : %d", todayPlayGameCount).c_str());
	m_staticTotalGames.SetWindowText(common::formatw("Total Game Count : %d", totalGames).c_str());
}


void CVRSimDlg::OnBnClickedRadioAxis3()
{
	UpdateData();

	if (m_selectGame>=0)
		m_plugins[m_selectGame].MotionSetSymbol("@yaw_speed", (float)m_radioAxisType);
}


void CVRSimDlg::OnBnClickedRadioAxis4()
{
	UpdateData();

	if (m_selectGame >= 0)
		m_plugins[m_selectGame].MotionSetSymbol("@yaw_speed", (float)m_radioAxisType);
}


void CVRSimDlg::OnClose()
{
	m_findProc.Clear();

	WriteConfigFile();

	SendServoMessage(SERVO_STOP);
	SendServoMessage(SERVO_OFF);

	SAFE_DELETE(m_image);
	Gdiplus::GdiplusShutdown(m_gdiPlusToken);

	CDialogEx::OnClose();
}


void CVRSimDlg::OnEnChangeEditLimitMinute()
{
	UpdateData();
	const float limitTime = (float)_ttof((LPCTSTR)m_editLimitTime);
	if (m_selectGame >= 0)
		m_plugins[m_selectGame].MotionSetSymbol("@limit_time", limitTime);
}
