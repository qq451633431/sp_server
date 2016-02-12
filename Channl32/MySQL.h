#ifndef __MYSQL_H__
#define __MYSQL_H__

#include "main.h"
#include "ServerEnum.h"
#include "ClientPackets.h"
#include "ServerPackets.h"
#include <mysql.h>

class MySQL
{
private:
MYSQL *connection;

public:
MySQL();
~MySQL();
void SetUserIP(char*,char*);
void GetUserIP(int,char*);
void GetUserInfo(char*,MyCharInfo&);
void GetUserItems(int id,bool*,int*,int*,int*,int*,int*);
int GetValidSlot(int,int);
void InsertNewItem(MyCharInfo*,int,int,int,int);
void GetShopItemCost(int,int,int,int&,int&);
int DeleteItem(int,int);
void InsertMsg(char *,char *, char *);
void ChangeEquips(int,EquipChangeRequest*);
void GetUserData(UserInfoResponse*);
void GetEquipData(int,RoomPlayerDataResponse*);
void GetMoneyAmmount(int,int*,unsigned __int64*,char=0,int=0,unsigned __int64=0);
void UpgradeCard(MyCharInfo *,CardUpgradeResponse*);
void GetScrolls(MyCharInfo *);
void UpdateScrolls(int id,int slot,int scroll);
void IncreaseMission(int id,int n);
void SearchShop(CardSearchResponse*,SearchType);
void GetExp(int usr_id, int usr_exp,const char *Elements=0, int Ammount=0);
};

struct LobbyUser
{
    string name;
    bool gender;
    int level, mission;
    LobbyUser *ptr;
};

class LobbyList
{
private:
    int count;
    LobbyUser *root;

public:
    LobbyList();
    void Insert(LobbyUser);
    void Delete(string);
    int GetList(unsigned char*);
};

#endif
