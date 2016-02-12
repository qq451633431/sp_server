#include "ChannelServer.h"
#include "ini.h"
CIni config("..\\config.ini", "CONFIG");
HANDLE hConsoleOutput;

int main()
{
    srand(time(0));
    //cout << hex << sizeof(QuestGainResponse) << endl;
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    ChannelServer *CS = new ChannelServer;

	config.SetSection("CHANNELS");
    int32 port = config.ReadInteger("port", 9303);

    if (CS->Start(port))
        cout << "----- Channel Server Started -----" << endl;

    CS->CommLoop();
    delete CS;

    cout << "Server Closing" << endl;
    cin.get();
    return 0;
}