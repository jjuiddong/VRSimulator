#define  _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "../../../../Common/Common/common.h"
#include "../../../../Common/Network/network.h"
#include <windows.h>
using namespace common;
using namespace std;

void main()
{
	network::cUDPServer server;
	server.Init(0, 10000);
	network::cUDPClient client;
	client.Init("127.0.0.1", 10001);

	cout << "udp server port 10000" << endl;
	cout << "udp client port 10001" << endl;

	while (1)
	{
		BYTE buffer[512];
		ZeroMemory(buffer, sizeof(buffer));
		const int len = server.GetRecvData(buffer, sizeof(buffer));
		if (len > 0)
		{
			vector<string> out;
			tokenizer((char*)buffer, ";", "", out);

// 			cout << "packet ---------------" << endl;
// 			for each (auto &str in out)
// 				cout << str << endl;

			BYTE sndBuffer[512];
			ZeroMemory(sndBuffer, sizeof(sndBuffer));
			int i = 0;
			for each (auto &str in out)
			{
				float f = (float)atof(str.c_str());
				memcpy(sndBuffer + i, &f, sizeof(f));
				i += sizeof(f);
			}
			client.SendData(sndBuffer, i);
		}
	}
}
