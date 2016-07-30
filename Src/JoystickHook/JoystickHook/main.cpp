

#include "../../../../Common/Common/common.h"
#include "../../../../Common/Network/network.h"
#include "../../../../Common/Motion/motion.h"

using namespace std;

void main()
{
	motion::cJoyPad joypad;
	if (!joypad.Init(NULL))
	{
		cout << "error init joypad" << endl;
		system("pause");
		return;
	}

	network::cUDPClient client;
	if (!client.Init("127.0.0.1", 8888))
	{
		cout << "error init udp client " << endl;
		system("pause");
		return;
	}

	cout << "success connect udp " << endl;

	int data[4];
	while (1)
	{
		joypad.Update(0);
		data[0] = joypad.m_axisX;
		data[1] = joypad.m_axisY;
		data[2] = joypad.m_axisRz;
		data[3] = joypad.m_axisRy;
		client.SendData((BYTE*)data, sizeof(data));

		Sleep(10);
	}
}
