
#include "stdafx.h"
#include "warthundermotioninput.h"

using namespace std;

int main() 
{
	motion::cWarThunderMotionInput wtMotionInput;
	wtMotionInput.Init();

	network::cUDPClient udpClient;
	if (!udpClient.Init("127.0.0.1", 8888))
	{
		cout << "udp connect error " << endl;
		system("pause");
		return 0;
	}

	cout << "success udp connect" << endl;

	float data[3];
	while (1)
	{
		wtMotionInput.GetMotion(data[0], data[1], data[2]);
		udpClient.SendData((BYTE*)data, sizeof(data));
		Sleep(100);
	}

	system("pause");
	return 0;
}
