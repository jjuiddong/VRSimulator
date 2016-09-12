
#include "global.h"
#include "warthundermotioninput.h"


using namespace graphic;


class cViewer : public framework::cGameMain
{
public:
	cViewer();
	virtual ~cViewer();

	virtual bool OnInit() override;
	virtual void OnUpdate(const float elapseT) override;
	virtual void OnRender(const float elapseT) override;
	virtual void OnShutdown() override;
	virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) override;


public:
	motion::cWarThunderMotionInput m_input;
	graphic::cCharacter m_cube;
	graphic::cLine m_lines[3]; // X,Y,Z
	Matrix44 m_carTm;

	Vector3 m_pos;
	POINT m_curPos;
	bool m_LButtonDown;
	bool m_RButtonDown;
	bool m_MButtonDown;
	Matrix44 m_rotateTm;
	Vector3 m_boxPos;
};

INIT_FRAMEWORK(cViewer);

const int WINSIZE_X = 640;
const int WINSIZE_Y = 480;

cViewer::cViewer()
{
	m_windowName = L"WarThunder Motion Viewer";
	const RECT r = { 0, 0, WINSIZE_X, WINSIZE_Y };
	m_windowRect = r;
	m_LButtonDown = false;
	m_RButtonDown = false;
	m_MButtonDown = false;
}

cViewer::~cViewer()
{
	graphic::ReleaseRenderer();
}


bool cViewer::OnInit()
{
	graphic::cResourceManager::Get()->SetMediaDirectory("./");

	GetMainCamera()->Init(&m_renderer);
	GetMainCamera()->SetCamera(Vector3(30, 30, 30), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GetMainCamera()->SetProjection(D3DX_PI / 4.f, (float)WINSIZE_X / (float)WINSIZE_Y, 1.f, 10000.0f);

	GetMainLight().Init(cLight::LIGHT_DIRECTIONAL);
	GetMainLight().SetPosition(Vector3(5, 5, 5));
	GetMainLight().SetDirection(Vector3(1, -1, 1).Normal());

	m_renderer.GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	m_renderer.GetDevice()->LightEnable(0, true);

	m_cube.Create(m_renderer, "cube.dat");
	const float len = 20;
	m_lines[0].SetLine(m_renderer, Vector3(-len, 0, 0), Vector3(len, 0, 0), 0.5f);
	m_lines[0].GetMaterial().InitRed();
	m_lines[1].SetLine(m_renderer, Vector3(0, -len, 0), Vector3(0, len, 0), 0.5f);
	m_lines[1].GetMaterial().InitGreen();
	m_lines[2].SetLine(m_renderer, Vector3(0, 0, -len*0.6f), Vector3(0, 0, len), 0.5f);
	m_lines[2].GetMaterial().InitBlue();

	m_input.Init();


	return true;
}


void cViewer::OnUpdate(const float elapseT)
{
	GetMainCamera()->Update();

	float roll, pitch, yaw;
	m_input.GetMotion(roll, pitch, yaw);
 	D3DXMatrixRotationYawPitchRoll((D3DXMATRIX*)&m_carTm, yaw, pitch*0.01f, roll*0.01f);
}


void cViewer::OnRender(const float elapseT)
{
	//화면 청소
	if (SUCCEEDED(m_renderer.GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(150, 150, 150), 1.0f, 0)))
	{
		m_renderer.GetDevice()->BeginScene();

		GetMainLight().Bind(m_renderer, 0);
		m_renderer.RenderGrid();
		m_renderer.RenderAxis();
		m_renderer.RenderFPS();

		m_cube.Render(m_renderer, m_carTm);
		for (int i = 0; i < 3; ++i)
			m_lines[i].Render(m_renderer, m_carTm);

		//랜더링 끝
		m_renderer.GetDevice()->EndScene();
		m_renderer.GetDevice()->Present(NULL, NULL, NULL, NULL);
	}
}


void cViewer::OnShutdown()
{

}


void cViewer::OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		dbg::Print("%d %d", fwKeys, zDelta);

		const float len = graphic::GetMainCamera()->GetDistance();
		float zoomLen = (len > 100) ? 50 : (len / 4.f);
		if (fwKeys & 0x4)
			zoomLen = zoomLen / 10.f;

		graphic::GetMainCamera()->Zoom((zDelta < 0) ? -zoomLen : zoomLen);
	}
	break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_BACK:
			break;
		case VK_TAB:
		{
			static bool flag = false;
			m_renderer.GetDevice()->SetRenderState(D3DRS_CULLMODE, flag ? D3DCULL_CCW : D3DCULL_NONE);
			m_renderer.GetDevice()->SetRenderState(D3DRS_FILLMODE, flag ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

		case VK_LEFT: m_boxPos.x -= 10.f; break;
		case VK_RIGHT: m_boxPos.x += 10.f; break;
		case VK_UP: m_boxPos.z += 10.f; break;
		case VK_DOWN: m_boxPos.z -= 10.f; break;
		}
		break;

	case WM_LBUTTONDOWN:
	{
		m_LButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_LBUTTONUP:
		m_LButtonDown = false;
		break;

	case WM_RBUTTONDOWN:
	{
		m_RButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
	}
	break;

	case WM_RBUTTONUP:
		m_RButtonDown = false;
		break;

	case WM_MBUTTONDOWN:
		m_MButtonDown = true;
		m_curPos.x = LOWORD(lParam);
		m_curPos.y = HIWORD(lParam);
		break;

	case WM_MBUTTONUP:
		m_MButtonDown = false;
		break;

	case WM_MOUSEMOVE:
	{
		if (m_RButtonDown)
		{
			POINT pos = { LOWORD(lParam), HIWORD(lParam) };
			const int x = pos.x - m_curPos.x;
			const int y = pos.y - m_curPos.y;
			m_curPos = pos;

			graphic::GetMainCamera()->Yaw2(x * 0.005f);
			graphic::GetMainCamera()->Pitch2(y * 0.005f);
		}
		else if (m_MButtonDown)
		{
			const POINT point = { LOWORD(lParam), HIWORD(lParam) };
			const POINT pos = { point.x - m_curPos.x, point.y - m_curPos.y };
			m_curPos = point;

			const float len = graphic::GetMainCamera()->GetDistance();
			graphic::GetMainCamera()->MoveRight(-pos.x * len * 0.001f);
			graphic::GetMainCamera()->MoveUp(pos.y * len * 0.001f);
		}

	}
	break;
	}
}

