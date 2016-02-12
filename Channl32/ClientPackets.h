#ifndef __CLIENTPACKETS_H__
#define __CLIENTPACKETS_H__

#define LOBBY_USERINFO_REQ    12000
#define JOIN_CHANNEL_REQ      17153
#define ROOM_CREATE_REQ       17159
#define ROOM_PLAYERDATA_REQ   17425
#define IN_ROOM_REQ           17173
#define NPC_LIST_REQ          17250
#define NPC_KILL              17240
#define QUEST_LIFE_REQ        17205
#define REVIVE_REQ            17203
#define EQUIP_CHANGE_REQ      17174
#define ROOM_EXIT_REQ         17176
#define ROOM_QUIT_REQ		  17186
#define CHAT_REQ              17201
#define USER_INFO_REQ         17161
#define ROOM_PLAYER_LIST_REQ  17169
#define ROOM_JOIN_REQ         17171
#define SHOP_JOIN_REQ         17251
#define SHOP_BUY_REQ          17255
#define SHOP_SELL_REQ         17253
#define CARD_UPGRADE_REQ      17218
#define IN_GAME_REQ           17474
#define PLAYER_KILLED_REQ     17205
#define TRADE_REQ             17216
#define LOBBY_USR_SHOP_REQ    17458
#define BUY_SCROLL_REQ        17193
#define QUEST_PROG_REQ		  17248
#define USE_SCROLL_REQ		  17209
#define PICK_SCROLL_REQ		  17478
#define MISSION_COMPLETE_REQ  17494
#define OPEN_USER_SHOP_REQ	  17265
#define CARD_SEARCH_REQ		  17458
#define BIGBATTLE_NPC_KO_REQ  17302
#define PLAYER_KICK_REQ		  17184
#define CHANGE_ROOMTITLE_REQ  17232

struct JoinChannelRequest
{
    int size;
    int type;
    int unk1; //11036
    int checksum;
    int state;
    char username[13];
    char unkn[13];
    int unk2; //2
    short unk3; //0x0C10
    int unk4; //0x9D7603C9
    char unkn1[9];
    int unk5; //0x1068 PID
    int unk6; //7
    int unk7; //0x13
    int unk8;
    int unk9;
};

struct CreateRoomRequest
{
    int size;
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int roomnumber;
    char roomname[32];
    int mode; //0x1b
    int map; //0x1b
    char password[16];
    int capacity;
    char allowscrolls; //1
    char autoteam; //0
    short unk2; //0
    int maxcardlevel; //-1
    char allowcritsheild;
    char unk3; //0
    char unk4; //0
    char unk5; //0
};

struct RoomPlayerDataRequest
{
    int size; //0x38
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2;
    int unk3;
    int unk4;
    int unk5;
    int unk6;
    int unk7;
    int bbbb1; //0x01010101
    int bbbb2; //0x01010101
    int unk8; //0x18B17401
};

struct NpcData
{
	short xPos;
	short yPos;
	int unk1;
	int unk2;
	short unk3;
	short unk4;
	short one;
	short npcType;
	short one2;
	short level;
};

struct NpcList
{
    int size; //0x404
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2;
	int unk3;
	NpcData npc[33];
	char idc[208];
};

struct ChatRequest
{
    int size; //0x7C
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int chatType; //0 All, 7 Trade
    char senderId[13];
    char recieverId[13];
    char msg[52];
    short unk2;
    int unk3;
    int unk4; //1
    int unk5;
    int unk6; //0x0018F9C4
    int unk7; //0x74D2FF16
};

struct UserInfoRequest
{
    int size; //0x28
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2;
    char username[16];
};

struct EquipChangeRequest
{
    int size; //0x40
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int mag;
    int wpn;
    int arm;
    int pet;
    int foot;
    int body;
    int hand1;
    int hand2;
    int face;
    int hair;
    int head;
};

struct RoomPlayerListRequest
{
    int size; //0x18
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int roomnumber;
};

struct RoomJoinRequest
{
    int size; //0x28
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int roomnumber;
    char roompw[16];
};

struct ShopBuyRequest
{
    int size; //0x2C
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2; //-1
    int item;
    int gf;
    int unk3;
    int level;
    int unk5; //1
};

struct ShopSellRequest
{
    int size; //0x18
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int item_slot;
};

struct InRoomRequest
{
    int size; //0x30
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2; //0
    int Character;
    int team; //10
    int Ready; //1
    int GameStart; //2
    int unk3;
    int unk4;
};

struct CardUpgradeRequest
{
    int size; //0x20
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int Slot;
    int UpgradeType; //1 = Lvl, 2 = Skill
    int unk3; //0
};

struct PlayerKilledRequest
{
    int size; //0x3C
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int unk2; //Player slot?
    int killerslot;
    int unk3;
    int points;
    int unk4;
    int unk5;
    int unk6;
    int unk7;
    int unk8;
    int unk9;
};

struct TradeStruct
{
    int size; //0x98
    int type;
    int unk1; //11036
    int checksum;
    int state;
    char username[16];
    int confirm; //1 = None, 5 = Cancel, 6 = Add Item, 7 = Config, 9 = Final Confirm, 14 = Trade Fail
    char username2[16];
    int itemID[4];
	int Code;
    int zeros[2];
	int itemType[4];
	int itemGF[4];
	int itemLevel[4];
    int unk08;
};

struct LobbyUsrShopResuqest
{
    int size; //0x20
    int type;
    int unk1; //11036
    int checksum;
    int state;
    int itemtype; //-1
    int itemlevel; //-1
    int item; //Shield = 21,Armor = 23,Pendant = 22
};

struct ScrollBuyRequest
{
    int size; //0x1c
    int type;
    int unk1; //11036
    int checksum;
	int state;
    int buy; //0
    int scrollid; //0xB = bubble
};

struct ScrollUseRequest
{
    int size; //0x24
    int type;
    int unk1; //11036
    int checksum;
	int state;
    int unk2; //2
    int slot;
	int unk3; //1
	int unk4;
};

struct PickScrollRequest
{
    int size; //0x20
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int zero;
	int scroll;
	int slot;
};

struct QuestProgress
{
    int size; //0x1c
    int type;
    int unk1; //11036
    int checksum;
	int state;
    int unk2; //1
	int progress; //0~100
};

struct InGameRequest
{
    int size; //0x20
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int request;
	int zero1;
	int zero2;
};

struct MissionCompleteRequest
{
    int size; //0x18
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int request;
};

struct OpenUserShopRequest
{
    int size; //0xC8
    int type;
    int unk1; //11036
    int checksum;
	int state;
	char id[12];
	char username[12];
	char title[17];
	int unk2;
	int unk3;
	int unk4;
	int numberofitems; //maybe
	int unk01; //-1
	int items[4];
	int unks[16]; //-1
	int unk5;
	unsigned __int64 price[4];
};

struct CardSearchRequest
{
    int size; //0x20
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int characteristic;
	int level;
	SearchType searchtype;
};

struct BigBattleNpcKoRequest
{
    int size; //0x40
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int killer; //0 = npc, other = player
	int npcn;
	int npclevel;
	int unk2;
	int killerid;
	int unk3;
	char unk4[20];
};

struct NpcKillRequest
{
    int size; //0x48
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int npc_num;
	int zero;
	int timeStamp;
	int damages[8];
	short npcType;
	short npcLevel;
	int unk2;
};

struct PlayerKickRequest
{
    int size; //0x18
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int slot;
};

struct RoomTitleChangeRequest
{
    int size; //0x38
    int type;
    int unk1; //11036
    int checksum;
	int state;
	int room;
	char title[32];
};

#endif
