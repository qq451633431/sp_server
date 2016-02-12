#ifndef __UDPSERVER_H__
#define __UDPSERVER_H__
#include "main.h"
#include "PacketHandler.h"
#include "UDPList.h"
#include "ini.h"

extern CIni config;

class UDPServer
{
private:
    unsigned char buffer[2000];
    WSADATA wsaData;
    struct sockaddr_in server, client;
    int serverlen, clientlen;
    SOCKET listen_socket, msg_socket;
    int port;
    IOSocket cIOSocket;

public:
    UDPServer()
	{
		for (int i = 0; i < 2000; i++)
			buffer[i] = 0;

		serverlen = sizeof(server);
		clientlen = sizeof(client);
	}

	~UDPServer()
	{
		WSACleanup();
	}

	bool Start()
	{
		if (WSAStartup(514, &wsaData))
		{
			cout << "WSAStartup Error#" << WSAGetLastError() << endl;
			return false;
		}

		server.sin_family = 2;
		uint16 port = config.ReadInteger("port", 9303, "UDPSERVER");
		server.sin_port = htons(port);
		server.sin_addr.s_addr = 0;

		listen_socket = socket(PF_INET,SOCK_DGRAM,0);

		if (listen_socket == INVALID_SOCKET)
		{
			cout << "Channel Server: listen_socket Error#" << WSAGetLastError() << endl;
			return false;
		}

		if (bind(listen_socket,(struct sockaddr*)&server, serverlen) == SOCKET_ERROR)
		{
			cout << "Channel Server: bind Error#" << WSAGetLastError() << endl;
			return false;
		}

		return true;
	}

	bool CommLoop()
	{
		MySQL MySql;
		bool bExit = false, sclosed = false;
		int retbufsize = 0;
		while (!bExit)
		{
			msg_socket = listen_socket;
			retbufsize = recvfrom(msg_socket,(char*)buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &clientlen);

			if(buffer[0] == 0x34)
			{
				for (int i = 4; i < *(int*)buffer; i++)
					buffer[i] = ~((BYTE)(buffer[i] >> 3) | (BYTE)(buffer[i] << 5));
				char name[16],IP[16];
				strcpy(name,(const char*)(buffer+0x14));
				strcpy(IP,(const char*)(buffer+0x24));
				MySql.SetUserIP(name,IP);
				HandleList.updateUdpClient(name,client);
				*(int*)(buffer+0x10) = HandleList.updateUdpState(name);
				*(int*)(buffer+0x0C) = cIOSocket.MakeDigest((uint8*)buffer);
				for (int i = 4; i < *(int*)buffer; i++)
					buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
				retbufsize = sendto(msg_socket,(char*)buffer, 0x34, 0, (struct sockaddr *)&client, clientlen);
			}else 
				if(buffer[0] == 0x2c)
				{
					unsigned char Send1[] = {0x2c,0x0,0x0,0x0,0x0,0x11,0x0,0x0,0x1c,0x2b,0x0,0x0,0xc2,0x6d,0x80,0xa1,0xc9,0xc9,0x83,0x4,0x53,0x70,0x72,0x31,0x6e,0x67,0x0,0x0,0x60,0xbc,0x12,0x0,0x65,0x35,0x37,0x0,0xce,0xc7,0xf0,0x17,0x90,0x5c,0x8d,0x4};
					for (int i = 4; i < *(int*)buffer; i++)
						buffer[i] = ~((BYTE)(buffer[i] >> 3) | (BYTE)(buffer[i] << 5));
					char name[16];
					strcpy(name,(const char*)(buffer+0x14));
					//*(int*)(buffer+0x14) = 0;
					*(int*)(Send1+0x10) = HandleList.updateUdpState(name);
					*(int*)(Send1+0x0C) = cIOSocket.MakeDigest((uint8*)Send1);
					for (int i = 4; i < *(int*)Send1; i++)
						Send1[i] = ~((BYTE)(Send1[i] << 3) | (BYTE)(Send1[i] >> 5));
					retbufsize = sendto(msg_socket,(char*)Send1, 0x2C, 0, (struct sockaddr *)&client, clientlen);
				}else {
					for (int i = 4; i < *(int*)buffer; i++)
						buffer[i] = ~((BYTE)(buffer[i] >> 3) | (BYTE)(buffer[i] << 5));
					int room = *(int*)(buffer+0x14);
					int source = *(int*)(buffer+0x18);
					int dest = *(int*)(buffer+0x1C);
					for (int i = 4; i < *(int*)buffer; i++)
						buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
					sockaddr_in *udp_client = RoomList.GetClientUdp(room,dest);
					if(udp_client)
						retbufsize = sendto(msg_socket,(char*)buffer, *(int*)buffer, 0,(struct sockaddr *)udp_client, clientlen);
				}
		}

		closesocket(msg_socket);
		return true;
	}
};

void PacketHandler::startUDP()
{
    UDPServer UDPServ;

    if (UDPServ.Start())
    {
        printf("----- UDP Server Started -----\n");
        UDPServ.CommLoop();
    }
    else printf("----- UDP Server Error -----\n");
}

struct udp_handshake {
    int size; //0x34
    int type; //0x1101
    int unk1; //11036
    int checksum;
    int state;
	int unk01; //0x00373700
	int unk02; //0x29
	int unk03; //0x0012FC74
	int unk04; //0x00510918
	int unk05; //0x0012FC00
	int unk06; //0x0012BC60
	int unk07; //0x00373565
	int unk08; //0x0012BC60
};


#endif
