#ifndef __CHANNELSERVER_H__
#define __CHANNELSERVER_H__

#include "main.h"
#include "PacketHandler.h"

class ChannelServer
{
private:
    //unsigned char buffer[9000];
    WSADATA wsaData;
    struct sockaddr_in server, client;
    int serverlen, clientlen;
    SOCKET listen_socket, msg_socket;
    int port;

public:
    ChannelServer();
    ~ChannelServer();
    bool Start(int);
    bool CommLoop();
    void outBuffer();
};


#endif
