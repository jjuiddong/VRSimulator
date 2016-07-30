#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "plugin.h"


class CVRSimDlg : public CDialogEx
{
public:
	CVRSimDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_VRSIM_DIALOG };
	bool Run();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void ReadConfigFile();
	void WriteConfigFile();
	void SendServoMessage(const int cmd);
	void MainLoop(const float deltaSeconds);
	void DetectGameLoop(const float deltaSeconds);
	void MotionLoop(const float deltaSeconds);
	void MotionEndLoop(const float deltaSeconds);
	bool IsLiveGame(const string &gameName);


protected:
	enum STATE { DETECT_GAME, MOTION_LOOP, MOTION_END_LOOP};

	STATE m_state;	
	int m_selectGame;
	HICON m_hIcon;
	bool m_isLoop;
	float m_delaySeconds;
	bool m_isViewDetails;
	vector<sPluginInfo> m_plugins;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CStatic m_staticFPS;
	afx_msg void OnBnClickedButtonDetails();
	CButton m_buttonDetails;
	afx_msg void OnBnClickedButtonServoon();
	afx_msg void OnBnClickedButtonServooff();
	afx_msg void OnBnClickedButtonServostart();
	afx_msg void OnBnClickedButtonServostop();
	CSliderCtrl m_sliderSpeed;
	float m_editSpeed;
	afx_msg void OnNMCustomdrawSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_strGameTitle;
	afx_msg void OnBnClickedButtonUpdate();
};
