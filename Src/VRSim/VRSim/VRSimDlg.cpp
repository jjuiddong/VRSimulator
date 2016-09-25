
#include "stdafx.h"
#include "VRSim.h"
#include "VRSimDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CVRSimDlg::CVRSimDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VRSIM_DIALOG, pParent)
	, m_isLoop(true)
	, m_state(DETECT_GAME)
	, m_delaySeconds(0)
	, m_isViewDetails(false)
	, m_editSpeed(0)
	, m_selectGame(-1)
	, m_strGameTitle(_T(""))
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
}

BEGIN_MESSAGE_MAP(CVRSimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//UI ----------------------------------------------------------------
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

	list<string> exts;
	exts.push_back("dll");
	list<string> files;
	common::CollectFiles(exts, "./Plugins/", files);

	for each (auto file in files)
	{
		sPluginInfo plugin;
		if (plugin.Load(file))
		{
			m_plugins.push_back(plugin);
		}
		else
		{
			::AfxMessageBox(formatw("Error!! Load Plugin [%s] errorCode=%d \n", file.c_str()).c_str(), GetLastError() );
			return FALSE;
		}
	}

	return TRUE;
}

void CVRSimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
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
	for (u_int i=0; i < m_plugins.size(); ++i)
	{
		if (IsLiveGame(m_plugins[i].gameName))
		{
			m_selectGame = i;
			m_state = MOTION_LOOP;
			m_strGameTitle = str2wstr(m_plugins[i].outputGameName).c_str();

			// 게임 발견.  초기화
			if (m_plugins[i].MotionInit((int)m_hWnd) <= 0)
			{
				::AfxMessageBox(L"Error Motion Init \n");
			}

			SetBackgroundColor(g_blueColor);
			UpdateData(FALSE);
			break;
		}
	}
}


void CVRSimDlg::MotionLoop(const float deltaSeconds)
{
	if (m_plugins.empty() || (m_selectGame < 0))
	{
		goto not_detect_game;
	}

	if (!IsLiveGame(m_plugins[m_selectGame].gameName))
	{
		goto not_detect_game;
	}

	m_plugins[m_selectGame].MotionUpdate(deltaSeconds);
	return;


not_detect_game:
	m_state = MOTION_END_LOOP;
	m_strGameTitle = "";
	m_plugins[m_selectGame].MotionEnd();
	SetBackgroundColor(g_grayColor);
	UpdateData(FALSE);
}


void CVRSimDlg::MotionEndLoop(const float deltaSeconds)
{
	if (m_plugins.empty())
		return;

	const int result = m_plugins[m_selectGame].MotionUpdate(deltaSeconds);
	if (result == 0)
	{ // motion device finish
		m_state = DETECT_GAME;
		m_plugins[m_selectGame].MotionClear();
		UpdateData(FALSE);
	}
}


// check running game
bool CVRSimDlg::IsLiveGame(const string &gameName)
{
	const HWND hwnd = ::FindWindowA(NULL, gameName.c_str());
	return hwnd ? true : false;
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
	SendServoMessage(1);
}


void CVRSimDlg::OnBnClickedButtonServooff()
{
	SendServoMessage(2);
}


void CVRSimDlg::OnBnClickedButtonServostart()
{
	SendServoMessage(3);
}


void CVRSimDlg::OnBnClickedButtonServostop()
{
	SendServoMessage(4);
}


static const string g_ConfigFileName = "./config.ini";
// config.ini 파일로 부터 axis, motion speed 값을 가져온다.
void CVRSimDlg::ReadConfigFile()
{
	const float motionSpeed = uiutil::GetProfileFloat("config", "motion_speed", 1.f, g_ConfigFileName);
	const float yawSpeed = uiutil::GetProfileFloat("config", "yaw_speed", 0.f, g_ConfigFileName);

// 	m_AxisRadio = (yawSpeed > 0) ? 1 : 0;
 	m_editSpeed = motionSpeed;
 	m_sliderSpeed.SetPos((int)(motionSpeed * 1000.f));
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
 
 	ofs << "[config]" << endl;
// 	ofs << "yaw_speed = " << m_AxisRadio << endl;
 	ofs << "motion_speed = " << pos << endl;
}


void CVRSimDlg::SendServoMessage(const int cmd)
{
// 	if (motion::cOutput *output = motion::cController2::Get()->GetOutput(0))
// 	{
// 		output->Start();
// 		output->SetFormat(cmd);
// 		for (int i = 0; i < 5; ++i)
// 		{
// 			output->SendImmediate();
// 			Sleep(100);
// 		}
// 	}
}


void CVRSimDlg::OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	const float pos = (float)m_sliderSpeed.GetPos() / 1000.f;
	m_editSpeed = pos;
	UpdateData(FALSE);
// 	script::g_symbols["@motion_speed"].type = script::FIELD_TYPE::T_FLOAT;
// 	script::g_symbols["@motion_speed"].fVal = pos;
	*pResult = 0;
}


void CVRSimDlg::OnBnClickedButtonUpdate()
{
	if (m_selectGame >= 0)
		m_plugins[m_selectGame].MotionUpdateScript();
}
