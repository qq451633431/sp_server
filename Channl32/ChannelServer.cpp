#include "ChannelServer.h"
#include "console.h"

IOSocket cIOSocket2;

ChannelServer::ChannelServer()
{
    //for (int i = 0; i < BUFFER_SIZE; i++)
    //  buffer[i] = 0;

    serverlen = sizeof(server);
    clientlen = sizeof(client);
}

ChannelServer::~ChannelServer()
{
    WSACleanup();
//    delete buffer;
}

bool ChannelServer::Start(int port)
{
    if (WSAStartup(514, &wsaData))
    {
        MakeMeFocused("WSAStartup Error",0);
        return false;
    }
    else MakeMeFocused("WSAStartup success ",1);

    server.sin_family = 2;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = 0;

    listen_socket = socket(2,1,0);

    if (listen_socket == INVALID_SOCKET)
    {
        MakeMeFocused("Channel Server: listen_socket Error",0);
        return false;
    }
    else MakeMeFocused("Channel Server: listen_socket success",1);

    if (bind(listen_socket,(struct sockaddr*)&server, serverlen) == SOCKET_ERROR)
    {
        MakeMeFocused("Channel Server: bind Error",0);
        return false;
    }
    else MakeMeFocused("Channel Server: bind success",1);


    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        MakeMeFocused("Channel Server: listen Error",0);
        return false;
    }
    else MakeMeFocused("Channel Server: listen success",1);

    return true;
}

struct _args {
SOCKET msg_socket;
char LastIP[20];
};

void Comm(void *args)
{
	_args *a = (_args*)args;
    {
        SOCKET msg_socket = (SOCKET)a->msg_socket;
        PacketHandler PackHandle(msg_socket,a->LastIP);
        int retbufsize = 0,n = 0;
        unsigned char *buffer = new unsigned char [9000];
        while(msg_socket)
        {
            retbufsize = recv(msg_socket, (char*)buffer, 9000, 0);

            if (!retbufsize)
            {
                printf("Channel Server: Connection closed by client\n");
                closesocket(msg_socket);
                break;
            }

            if (retbufsize == SOCKET_ERROR)
            {
                MakeMeFocused("Channel Server: Client socket closed\n",0);
                closesocket(msg_socket);
                break;
            }
            else printf("recv %d bytes success\n",retbufsize);


			int32 sz = *(int32*)buffer;
			if(sz != retbufsize)
			{
				MakeMeFocused("Channel Server: sz != retbufsize\n",0);
			}
			//ASSERT(sz == retbufsize);

            if(n == 0)
            {
                n = 1;
                PackHandle.Handle(buffer);
                printf("Channel Server: Sending First Response\n");
                retbufsize = send(msg_socket,(char*)buffer,PackHandle.ServerResponse(buffer)*buffer[0], 0);

                PackHandle.GenerateResponse(JOIN_MISSIONLEVEL_RESPONSE);
                PackHandle.ServerResponse(buffer);
                retbufsize = send(msg_socket,(char*)buffer,*(int*)buffer, 0);

                PackHandle.GenerateResponse(JOIN_PLAYERDATA_RESPONSE);

                memcpy(buffer,(unsigned char*)&PackHandle.Join_Channel_PlayerData_Response,0x980);
                *(int*)(buffer+0xc) = cIOSocket2.MakeDigest((uint8*)buffer);

                for (int i = 4; i < (*(int*)buffer); i++)
                    buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));

                retbufsize = send(msg_socket,(char*)buffer,0x980, 0);

                PackHandle.GenerateResponse(LOBBY_USERINFO_RESPONSE);
                int x = PackHandle.ServerResponse(buffer);
                send(msg_socket,(char*)buffer,x, 0);

                PackHandle.GenerateResponse(ROOM_LIST_RESPONSE);
                PackHandle.ServerResponse(buffer);
                send(msg_socket,(char*)buffer,*(int*)buffer, 0);

                continue;
            }


            PackHandle.Handle(buffer);
            if(PackHandle.nOfPackets)
            {
                MakeMeFocused("Channel Server: Sending Response",1);
                int x = PackHandle.ServerResponse(buffer);
                if(x < 10)
                    retbufsize = send(msg_socket,(char*)buffer,*(int*)buffer, 0);
                else retbufsize = send(msg_socket,(char*)buffer,x, 0);

            }
            else MakeMeFocused("Channel Server: Server have no response",0);
        }
        delete buffer;
    }
    _endthread();
}

bool ChannelServer::CommLoop()
{
    bool bExit = false;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PacketHandler::startUDP, 0, 0, 0 );
	_args args;
    while (!bExit)
    {
        if ((msg_socket = accept(listen_socket, (struct sockaddr*)&client, &clientlen)) == INVALID_SOCKET)
        {
            MakeMeFocused("Channel Server: Accept Error",0);
            return false;
        }
        else
        {
			args.msg_socket = msg_socket;
            strcpy(args.LastIP,inet_ntoa(client.sin_addr));
            printf("Channel Server: Accept Client with IP:%s\n",args.LastIP);
        }
        _beginthread((void (*)(void *))Comm,0,(void *)&args);
		Sleep(50);
    }

    return true;
}

void ChannelServer::outBuffer()
{
    /*
    printf("---- Recieved Data From %s ----\n",inet_ntoa(client.sin_addr));

    for (int i = 0; i < buffer[0]; i++)
    {
        if (i && i%16 == 0)printf("\n");
        if (*(BYTE*)(buffer+i) < 0x10)printf("0");
        printf("%x ",(int)*(BYTE*)(buffer+i));
    }
    printf("\n");
    */
}

