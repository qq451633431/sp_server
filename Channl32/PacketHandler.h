#ifndef __PACKETHANDLER_H__
#define __PACKETHANDLER_H__

#include "main.h"
#include "IOSocket.h"
#include "ServerEnum.h"
#include "ClientPackets.h"
#include "ServerPackets.h"
#include "MySQL.h"

class PacketHandler
{
private:
	struct sockaddr_in udp_client;
    MySQL MySql;
    SOCKET msg_socket;
    unsigned char* buffer;
    char IP[16],LocIP[16];
    bool Joined,Started;
	int LastPosition;

// Client Packet Structures
    JoinChannelRequest *Join_Channel_Request;
    CreateRoomRequest *Create_Room_Request;
    RoomPlayerDataRequest *Room_PlayerData_Request;
    ChatRequest *Chat_Request;
    UserInfoRequest *User_Info_Request;
    EquipChangeRequest *Equip_Change_Request;
    RoomPlayerListRequest *Room_PlayerList_Request;
    RoomJoinRequest *Room_Join_Request;
    ShopBuyRequest *Shop_Buy_Request;
    ShopSellRequest *Shop_Sell_Request;
    InRoomRequest *In_Room_Request;
    CardUpgradeRequest *Card_Upgrade_Request;
    PlayerKilledRequest *Player_Killed_Request;
    NpcList *Npc_List;
    ScrollBuyRequest *Scroll_BuyRequest;
	ScrollUseRequest *Scroll_UseRequest;
	PickScrollRequest *PickScroll_Request;
	QuestProgress *Quest_Progress;
	InGameRequest *InGame_Request;
	MissionCompleteRequest *MissionComplete_Request;
	OpenUserShopRequest *OpenUserShop;
	CardSearchRequest *CardSearch_Request;
	BigBattleNpcKoRequest *BigBattleNpcKo_Request;
	NpcKillRequest *NpcKill_Request;
	PlayerKickRequest *PlayerKick_Request;
	RoomTitleChangeRequest *RoomTitleChange_Request;

// Server Packet Structures
    JoinChannelResponse Join_Channel_Response;
    JoinChannelMissionLevelResponse Join_Channel_MissionLevel_Response;
    CreateRoomResponse Create_Room_Response;
    UserInfoResponse User_Info_Response;
    RoomListResponse Room_List_Response;
    RoomPlayerListResponse Room_PlayerList_Response;
    RoomJoinResponse Room_Join_Response;
    AfterRoomJoinResponse After_Room_JoinResponse;
    RoomExitResponse Room_Exit_Response;
    ShopJoinResponse Shop_Join_Response;
    ShopBuyResponse Shop_Buy_Response;
    ShopSellResponse Shop_Sell_Response;
    CardUpgradeResponse Card_Upgrade_Response;
    NewKingResponse NewKing_Response;
    RoundClearResponse RoundClear_Response;
    TradeStruct Trade_Struct;
    ReviveResponse *Revive_Response;
    BuyScrollResponse BuyScroll_Response;
	PickScrollResponse PickScroll_Response;
	SpawnResponse Spawn_Response;
	MissionCompleteResponse MissionComplete;
	MissionAfterResponse MissionAfter_Response;
	CardSearchResponse CardSearch_Response;
	BigBattleNpcKoResponse BigBattleNpcKo_Response;
	QuestGainResponse QuestGain_Response;
	NpcListResponse NpcList_Response;


public:
    JoinChannelPlayerDataResponse Join_Channel_PlayerData_Response;
    PacketHandler(SOCKET,char *);
    void Handle(unsigned char *);
    ~PacketHandler();
	void UpdateUDPClient(sockaddr_in);
	sockaddr_in* GetClientUdp();
    int UpdateState();
	int UpdateUDPState();
    int UpdateState(int);
    int IdentifyPacketType();
    int IdentifyPacketType(unsigned char*);
    void Encrypt(unsigned char*);
    void Decrypt(unsigned char*);
    void Encrypt(unsigned char*,int,bool = 0);
    void Decrypt(unsigned char*,int,bool = 0);
    void SetCard(int, int, Card, int, int, Skill);
    void SetMyCards();
    void GenerateResponse(int);
    char* ServerResponse();
    int ServerResponse(unsigned char*);
    void SetClientIP(char *);

    int nOfPackets, SendList, State, udpState;
    RoomPlayerDataResponse  Room_PlayerData_Response;
    unsigned char *pack;
    MyCharInfo Info;
    LobbyUser LobbyInfo;

	// Solve Udp linking
	static void startUDP();

	//Admin Functions
	void console(char*);
	void SendGmMessage(char*);
	void DisconnectPlayer(char*);
	void KickPlayer(char*);
	void RevivePlayer(char*);


    //SP_Functions
    void GetUserInfo();
    void GetUserItems();
    void GetChatMessage(ChatResponse*);
    void GetLobbyMessage(LobbyUserInfoResponse*);
    void GetNewRoomMessage(LobbyRoomResponse*);
    void GetExitRoomResponse();
    void GetPlayerExitRoomResponse(RoomExitResponse*);
    void GetInRoomData(RoomPlayerDataResponse*,bool);
    void GetInRoomPlayerList(RoomPlayerDataResponse*);
    void GetInRoomUpgradeResponse(CardUpgradeResponse*);
    void GetRoomListResponse();
	void GetRoomUpdateResponse(LobbyRoomResponse*);
    bool GetRoomJoinResponse();
    void GetRoomCreateResponse();
    void GetInRoomDeathResponse(PlayerKilledResponse*);
    void Trader();
    void GetTradeResponse(TradeStruct*);
    void GetRoomPlayerData();
	void GetNpcList(NpcList*);
    void GetMasterResponse(int);
    void GetExpGainResponse(QuestGainResponse*);
    void GetReviveResponse(ReviveResponse*);
	void GetResultResponse(ResultsResponse*);
	void GetBigBattleNpcMultiplier();
	void GetJoinResponse2();
	void GetKickResponse(int);
	void GetTitleChange(RoomTitleChangeResponse*);
};

//link to udp
void udpGetUserName(char *,int,int);

#endif
