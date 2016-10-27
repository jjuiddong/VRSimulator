
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "../../../../Common/Common/common.h"
#include "../../../../Common/Network/network.h"
#pragma comment(lib, "winmm.lib")

using namespace std;

network::cTCPClient client;

void SendTelemetry()
{
	// Big Endian
	unsigned int n = 0;
	BYTE sbuffer[10];
	ZeroMemory(sbuffer, sizeof(sbuffer));
	sbuffer[0] = 'N';
	*(unsigned short*)(sbuffer + 2) = 5;
	*(unsigned int*)(sbuffer + 6) = n;
	*(unsigned short*)(sbuffer + 7) = 0;
	sbuffer[9] = 'L';
	client.Send(sbuffer, sizeof(sbuffer));
}

void main(int argc, char **argv)
{
	if (argc < 3)
		return;

	string ip = argv[1];
	int port = atoi(argv[2]);

	if (!client.Init(ip, port, 86, 10, 30, true))
		return;
	network::cUDPClient udpClient;
	udpClient.Init("127.0.0.1", 10001, 30);

	cout << "connect" << endl;
	SendTelemetry();

	int t0 = timeGetTime();
	int state = 0; // 0=recv, 1=send
	while (1)
	{
		if (state == 0)
		{
			network::sSockBuffer buffer;
			if (client.m_recvQueue.Front(buffer))
			{
				if (buffer.readLen < 86)
				{
					client.m_recvQueue.Pop();
					SendTelemetry();
					continue;
				}

				cout << "recv len = " << buffer.readLen << endl;
				udpClient.SendData(buffer.buffer, buffer.readLen);
				client.m_recvQueue.Pop();
				state = 1;
			}
		}
		else if (state == 1)
		{
			if (timeGetTime() - t0 > 30)
			{
				t0 = timeGetTime();
				SendTelemetry();
				state = 0;
			}
		}
	}

}