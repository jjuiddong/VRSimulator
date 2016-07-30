
#include "stdafx.h"
#include "VRSim.h"
#include "VRSimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVRSimApp

BEGIN_MESSAGE_MAP(CVRSimApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CVRSimApp::CVRSimApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CVRSimApp theApp;

BOOL CVRSimApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CVRSimDlg *dlg = new CVRSimDlg();
	dlg->Create(CVRSimDlg::IDD);
	m_pMainWnd = dlg;

	dlg->Run();

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	delete dlg;
	return FALSE;
}

