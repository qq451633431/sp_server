#include "LoginServer.h"
#include "ini.h"
CIni config("..\\config.ini", "CONFIG");

HANDLE hConsoleOutput;

int main()
{
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	//printf("%x\n",sizeof(TrainingDoneResponse));
    cLoginServer *LoginServer = new cLoginServer;

    if (LoginServer->Start())
        printf("----- Server Started -----\n");

    LoginServer->CommLoop();
    delete LoginServer;

    printf("Server Closing\n");

    return 0;
}
