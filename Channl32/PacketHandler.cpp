#include "PacketHandler.h"
#include "SP_Functions.h"
#include "Admin_Functions.h"
#include "UDPServer.h"

ExpCalculator ExpCalc(10);

PacketHandler::PacketHandler(SOCKET S,char *ip)
{
	msg_socket = S;
	Joined = Started = false;
	LastPosition = SendList = 0;
	Info.usr_room = -1;
	Info.usr_ready = 0;
	Info.usr_team = 10;
	Info.premium = 120;
	if(Info.premium == 120)Info.questlife = true;
	Info.admin = true;
	for(int i = 0; i < 13; i++)Info.usr_name[i] = 0;
	for(int i = 0; i < 16; i++)LocIP[i] = 0;
	SetClientIP(ip);
	HandleList.Insert(this);
	pack = new unsigned char [9000];
}

void PacketHandler::Handle(unsigned char *buf)
{
	nOfPackets = 0;
	SendList = 0;
	Decrypt(buf);
	buffer = buf;

	switch (IdentifyPacketType(buf))
	{
	case JOIN_CHANNEL_REQ:
		Join_Channel_Request = (JoinChannelRequest*)buf;
		cout << "Client Trying to Join Channel with" << endl << "ID: " << Join_Channel_Request->username<< endl;
		cout << Join_Channel_Request->unkn << endl;
		GetUserInfo();
		Lobby.Insert(LobbyInfo);
		HandleList.ProdcastLobbyInfo(this,&LobbyInfo,true);
		GenerateResponse(JOIN_CHANNEL_RESPONSE);
		break;
	case SHOP_JOIN_REQ:
		cout << "SHOP_JOIN_REQ" << endl;
		GenerateResponse(SHOP_JOIN_RESPONSE);
		break;
	case SHOP_BUY_REQ:
		Shop_Buy_Request = (ShopBuyRequest*)buf;
		cout << "SHOP_BUY_REQ" << endl;
		GenerateResponse(SHOP_BUY_RESPONSE);
		break;
	case SHOP_SELL_REQ:
		Shop_Sell_Request = (ShopSellRequest*)buf;
		cout << "SHOP_SELL_REQ" << endl;
		GenerateResponse(SHOP_SELL_RESPONSE);
		break;
	case CARD_UPGRADE_REQ:
		Card_Upgrade_Request = (CardUpgradeRequest*)buf;
		cout << "CARD_UPGRADE_REQ" << endl;
		GenerateResponse(CARD_UPGRADE_RESPONSE);
		break;
	case USER_INFO_REQ:
		User_Info_Request = (UserInfoRequest*)buf;
		cout << "Client Requesting " << User_Info_Request->username << " Info" << endl;
		GenerateResponse(USER_INFO_RESPONSE);
		break;
	case EQUIP_CHANGE_REQ:
		Equip_Change_Request = (EquipChangeRequest*)buf;
		MySql.ChangeEquips(Info.usr_id,Equip_Change_Request);
		if(Info.usr_room != -1)GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
		cout << "EQUIP_CHANGE_REQ" << endl;
		break;
	case ROOM_PLAYER_LIST_REQ:
		Room_PlayerList_Request = (RoomPlayerListRequest*)buf;
		cout << "ROOM_PLAYER_LIST_REQ" << endl;
		GenerateResponse(ROOM_PLAYER_LIST_RESPONSE);
		break;
	case ROOM_JOIN_REQ:
		Room_Join_Request = (RoomJoinRequest*)buf;
		cout << "ROOM_JOIN_REQ" << endl;
		Info.usr_char = Info.DefaultCharacter + Info.premium;
		GenerateResponse(ROOM_JOIN_RESPONSE);
		break;
	case ROOM_CREATE_REQ:
		Create_Room_Request = (CreateRoomRequest*)buf;
		cout << "Client Trying to Create Room " << Create_Room_Request->roomnumber << endl;
		Info.usr_char = Info.DefaultCharacter + Info.premium;
		GenerateResponse(ROOM_CREATE_RESPONSE);
		break;
	case ROOM_PLAYERDATA_REQ:
		Room_PlayerData_Request = (RoomPlayerDataRequest*)buf;
		cout << "After Room Request" << endl;
		GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
		break;
	case CHANGE_ROOMTITLE_REQ:
		RoomTitleChange_Request = (RoomTitleChangeRequest*)buf;
		cout << "CHANGE_ROOMTITLE_REQ" << endl;
		GenerateResponse(CHANGE_ROOMTITLE_RESPONSE);
		break;
	case IN_ROOM_REQ:
		In_Room_Request = (InRoomRequest*)buf;
		cout << "IN_ROOM_REQ " << IP << " Team : " << In_Room_Request->team << endl;
		Info.usr_team = In_Room_Request->team;
		if(In_Room_Request->GameStart == 0)
		{
			Info.usr_char = In_Room_Request->Character;
			Info.usr_team = In_Room_Request->team;
			cout << "ROOM PROD: " << Info.usr_room << endl;
			HandleList.ProdcastRoomUpdate(Info.usr_room);
			GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
		}
		else if(In_Room_Request->GameStart == 2)
		{
			Info.usr_ready = (BYTE)In_Room_Request->Ready;
			//if(In_Room_Request->Ready > 10)
			//	Info.usr_ready = 0;
			GetBigBattleNpcMultiplier();
			GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
		}
		//GenerateResponse(NPC_LIST_RESPONSE);
		break;
	case IN_GAME_REQ:
		InGame_Request = (InGameRequest*)buf;
		cout << "IN_GAME_REQ" << endl;
		if(InGame_Request->request == 0xCB)
			GenerateResponse(SPAWN_RESPONSE);
		//GenerateResponse(IN_GAME_RESPONSE);
		break;
	case OPEN_USER_SHOP_REQ:
		OpenUserShop = (OpenUserShopRequest*)buf;
		cout << "OPEN_USER_SHOP_REQ" << endl;
		GenerateResponse(OPEN_USER_SHOP_REQ);
		break;
	case CARD_SEARCH_REQ:
		CardSearch_Request = (CardSearchRequest*)buf;
		cout << "CARD_SEARCH_REQ" << endl;
		GenerateResponse(CARD_SEARCH_RESPONSE);
		break;
	case MISSION_COMPLETE_REQ:
		MissionComplete_Request = (MissionCompleteRequest*)buf;
		cout << "MISSION_COMPLETE_REQ" << endl;
		if(MissionComplete_Request->request == 1)
			GenerateResponse(MISSION_COMPLETE_RESPONSE);
		else if(MissionComplete_Request->request == 2)
			GenerateResponse(MISSION_AFTER_RESPONSE);
		break;
	case PLAYER_KILLED_REQ:
		Player_Killed_Request = (PlayerKilledRequest*)buf;
		cout << "PLAYER_KILLED_REQ" << endl;
		RoomList.ProdcastDeathResponse(Player_Killed_Request,Info.usr_room);
		break;
	case TRADE_REQ:
		memcpy(&Trade_Struct,buf,sizeof(TradeStruct));
		Trader();
		break;
	case NPC_LIST_REQ:
		Npc_List = (NpcList*)buf;
		//RoomList.SetNpcList(Npc_List,Info.usr_room);
		RoomList.ProdcastNpcList(this,Npc_List,Info.usr_room);
		GenerateResponse(NPC_LIST_RESPONSE);
		break;
	case BUY_SCROLL_REQ:
		Scroll_BuyRequest = (ScrollBuyRequest*)buf;
		cout << "BUY_SCROLL_RESPONSE" << endl;
		GenerateResponse(BUY_SCROLL_RESPONSE);
		break;
	case PICK_SCROLL_REQ:
		PickScroll_Request = (PickScrollRequest*)buf;
		cout << "PICK_SCROLL_REQ" << endl;
		GenerateResponse(PICK_SCROLL_RESPONSE);
		break;
	case USE_SCROLL_REQ:
		Scroll_UseRequest = (ScrollUseRequest*)buf;
		cout << "USE_SCROLL_REQ" << endl;
		Info.scrolls[Scroll_UseRequest->slot] = 0;
		MySql.UpdateScrolls(Info.usr_id,Scroll_UseRequest->slot,0);
		break;
	case NPC_KILL:
		NpcKill_Request = (NpcKillRequest*)buf;
		cout << "NPC_KILL_RESPONSE -None" << endl;
		GenerateResponse(NPC_KILL_RESPONSE);
		break;
		/*    case QUEST_LIFE_REQ:
		cout << "QUEST_LIFE_RESPONSE" << endl;
		GenerateResponse(QUEST_LIFE_RESPONSE);
		break;
		*/
	case BIGBATTLE_NPC_KO_REQ:
		BigBattleNpcKo_Request = (BigBattleNpcKoRequest*)buf;
		cout << "BIGBATTLE_NPC_KO_REQ" << endl;
		GenerateResponse(BIGBATTLE_NPC_KO_RESPONSE);
		break;
	case PLAYER_KICK_REQ:
		PlayerKick_Request = (PlayerKickRequest*)buf;
		cout << "PLAYER_KICK_REQ" << endl;
		GenerateResponse(PLAYER_KICK_RESPONSE);
		break;
	case REVIVE_REQ:
		Revive_Response = (ReviveResponse*)buf;
		cout << "REVIVE_RESPONSE" << endl;
		GenerateResponse(REVIVE_RESPONSE);
		break;
	case ROOM_QUIT_REQ:
	case ROOM_EXIT_REQ:
		cout << "ROOM_EXIT/QUIT_RESPONSE" << endl;
		GenerateResponse(ROOM_EXIT_RESPONSE);
		break;
	case CHAT_REQ:
		Chat_Request = (ChatRequest*)buf;
		cout << "CHAT_REQ" << endl;
		GenerateResponse(CHAT_RESPONSE);
		break;
	case QUEST_PROG_REQ:
		Quest_Progress = (QuestProgress*)buf;
		cout << "QUEST_PROG_REQ" << endl;
		RoomList.UpdateProgress(Info.usr_room,Quest_Progress->progress);
		break;
	default:
		break;
	}
}

PacketHandler::~PacketHandler()
{
	delete pack;
	if(Info.usr_room)
		RoomList.ExitPlayer(Info.usr_room,this);
	Lobby.Delete(LobbyInfo.name);
	HandleList.ProdcastLobbyInfo(this,&LobbyInfo,false);
	HandleList.Delete(this);
}

void PacketHandler::UpdateUDPClient(sockaddr_in client)
{
	udp_client = client;
}

sockaddr_in* PacketHandler::GetClientUdp()
{
	return &udp_client;
}

int PacketHandler::UpdateState()
{
	return State = abs(((State = (~State + 0x14fb) * 0x1f) >> 16) ^ State);
}

int PacketHandler::UpdateUDPState()
{
	return udpState = abs(((udpState = (~udpState + 0x14fb) * 0x1f) >> 16) ^ udpState);
}

int PacketHandler::UpdateState(int S)
{
	return abs(((S = (~S + 0x14fb) * 0x1f) >> 16) ^ S);
}


int PacketHandler::IdentifyPacketType(unsigned char* buf)
{
	return *(int*)(buf+4);
}

void PacketHandler::Encrypt(unsigned char *buf)
{
	for (int i = 4; i < (*(int*)buf)*nOfPackets; i++)
	{
		buf[i] = ~((BYTE)(buf[i] << 3) | (BYTE)(buf[i] >> 5));
	}

}

void PacketHandler::Decrypt(unsigned char *buf)
{
	uint32 sz = *(int*)buf;
	if(sz >= 9000)
		sz = 9000-1;

	for (int i = 4; i < sz; i++)
		buf[i] = ~((BYTE)(buf[i] >> 3) | (BYTE)(buf[i] << 5));

	cout << "---- Decrypted Data  ----" << endl;
	for (int i = 0; i < sz; i++)
	{
		if (i && i%16 == 0)cout << endl;
		if (*(BYTE*)(buf+i) < 0x10)cout << '0';
		cout << hex << (int)*(BYTE*)(buf+i) << ' ';
	}
	cout << endl;
}

void PacketHandler::Encrypt(unsigned char *buf,int size,bool output)
{
	for (int i = 4; i < size; i++)
	{
		buf[i] = ~((BYTE)(buf[i] << 3) | (BYTE)(buf[i] >> 5));
	}
}

void PacketHandler::Decrypt(unsigned char *buf,int size, bool output)
{
	for (int i = 4; i < size; i++)
		buf[i] = ~((BYTE)(buf[i] >> 3) | (BYTE)(buf[i] << 5));

	if (!output)return;

	cout << "---- Decrypted Data  ----" << endl;
	cout << hex <<"00: ";
	for (int i = 0; i < size; i++)
	{
		if (i && i%16 == 0)cout << hex << endl << i <<": ";
		if (*(BYTE*)(buf+i) < 0x10)cout << '0';
		cout << hex << (int)*(BYTE*)(buf+i) << ' ';
	}
	cout << endl;
}

void PacketHandler::SetCard(int i, int ID, Card Type, int GF, int Level, Skill cSkill)
{
	Join_Channel_PlayerData_Response.bMyCard[i] = 1; //12x2x4
	Join_Channel_PlayerData_Response.IDMyCard[i] = ID;
	Join_Channel_PlayerData_Response.TypeMyCard[i] = Type.getType();
	Join_Channel_PlayerData_Response.GFMyCard[i] = GF;
	Join_Channel_PlayerData_Response.LevelMyCard[i] = Level;
	Join_Channel_PlayerData_Response.SkillMyCard[i] = cSkill.skill;
}

void PacketHandler::GenerateResponse(int ResponsePacketType)
{
	nOfPackets = 1;
	switch (ResponsePacketType)
	{
	case JOIN_CHANNEL_RESPONSE:
		Join_Channel_Response.size = 0x40;
		Join_Channel_Response.type = JOIN_CHANNEL_RESPONSE;
		Join_Channel_Response.unk1 = 11036;
		Join_Channel_Response.state = UpdateState();
		Join_Channel_Response.unk2 = 0; //0x2CF
		Join_Channel_Response.unk3 = 0; //0x2CA
		Join_Channel_Response.unk4 = 0; //0
		Join_Channel_Response.unk5 = 0; //1
		Join_Channel_Response.unk6 = 0;
		Join_Channel_Response.unk7 = 0;
		Join_Channel_Response.unk8 = 0;
		Join_Channel_Response.unk9 = 0; //0x6D15D3
		Join_Channel_Response.unk10 = 0; //0
		Join_Channel_Response.unk11 = 0; //0
		Join_Channel_Response.unk12 = 0; //0x78020F00
		Join_Channel_Response.checksum = cIOSocket.MakeDigest((uint8*)&Join_Channel_Response);
		buffer = (unsigned char*)&Join_Channel_Response;
		break;
	case JOIN_MISSIONLEVEL_RESPONSE:
		Join_Channel_MissionLevel_Response.size = 0x1C; //0x1C
		Join_Channel_MissionLevel_Response.type = JOIN_MISSIONLEVEL_RESPONSE;
		Join_Channel_MissionLevel_Response.unk1 = 11036;
		Join_Channel_MissionLevel_Response.state = UpdateState();
		Join_Channel_MissionLevel_Response.MissionLevel =  Info.Mission+1; //bug
		Join_Channel_MissionLevel_Response.unk2 = 0; //0
		Join_Channel_MissionLevel_Response.checksum = cIOSocket.MakeDigest((uint8*)&Join_Channel_MissionLevel_Response);
		buffer = (unsigned char*)&Join_Channel_MissionLevel_Response;
		break;
	case JOIN_PLAYERDATA_RESPONSE:
		memset(&Join_Channel_PlayerData_Response,0,sizeof(JoinChannelPlayerDataResponse));
		GetUserItems();
		Join_Channel_PlayerData_Response.size = 0x980; //0x980
		Join_Channel_PlayerData_Response.type = JOIN_PLAYERDATA_RESPONSE;
		Join_Channel_PlayerData_Response.unk1 = 11036; //11036
		Join_Channel_PlayerData_Response.state = UpdateState();
		Join_Channel_PlayerData_Response.unk2 = 1; //1
		Join_Channel_PlayerData_Response.unk3 = Info.gender*0x10000; //0x10000
		Join_Channel_PlayerData_Response.unk4 = 0x220; //0x220
		Join_Channel_PlayerData_Response.unk5 = 1; //1
		Join_Channel_PlayerData_Response.unk6 = 1; //1
		Join_Channel_PlayerData_Response.unk7 = 0; //0
		Join_Channel_PlayerData_Response.Points = Info.Points;
		Join_Channel_PlayerData_Response.Points2 = Info.Points;
		Join_Channel_PlayerData_Response.Code = Info.Code;
		Join_Channel_PlayerData_Response.Coins = Info.Coins;
		Join_Channel_PlayerData_Response.level1 = Info.Level;
		Join_Channel_PlayerData_Response.level2 = Info.Level;
		Join_Channel_PlayerData_Response.zero3[0] = Info.scrolls[0];
		Join_Channel_PlayerData_Response.zero3[1] = Info.scrolls[1];
		Join_Channel_PlayerData_Response.zero3[2] = Info.scrolls[2];
		Join_Channel_PlayerData_Response.WaterElements = Info.Water;
		Join_Channel_PlayerData_Response.FireElements = Info.Fire;
		Join_Channel_PlayerData_Response.EarthElements = Info.Earth;
		Join_Channel_PlayerData_Response.WindElements = Info.Wind;
		Join_Channel_PlayerData_Response.DefaultCharacter = Info.DefaultCharacter; //Maybe
		Join_Channel_PlayerData_Response.nOfSlots = Info.nSlots;
		Join_Channel_PlayerData_Response.unk8 = 0x32; //0x32
		Join_Channel_PlayerData_Response.unk9 = 0x64; //0x64
		Join_Channel_PlayerData_Response.unk10 = 0xC8; //0xC8
		Join_Channel_PlayerData_Response.unk11 = 0x190; //0x190
		Join_Channel_PlayerData_Response.unk12 = 0x320; //0x320
		Join_Channel_PlayerData_Response.unk13 = 0x640; //0x640
		Join_Channel_PlayerData_Response.unk14 = 0x960; //0x960
		Join_Channel_PlayerData_Response.unk15 = 0xC80; //0xC80
		Join_Channel_PlayerData_Response.unk16 = 0x1900; //0x1900
		Join_Channel_PlayerData_Response.unk17 = 0x32000; //0x32000
		Join_Channel_PlayerData_Response.unk18 = 0x64000; //0x64000
		Join_Channel_PlayerData_Response.unk19 = 0xC8000; //0xC8000
		Join_Channel_PlayerData_Response.unk20 = 0x190000; //0x190000
		Join_Channel_PlayerData_Response.unk21 = 0x320000; //0x320000
		Join_Channel_PlayerData_Response.unk22 = 0x640000; //0x640000
		Join_Channel_PlayerData_Response.unk23 = 0xC80000; //0xC80000
		Join_Channel_PlayerData_Response.unk24 = 0x1900000; //0x1900000
		Join_Channel_PlayerData_Response.unk25 = 0x3200000; //0x3200000
		Join_Channel_PlayerData_Response.unk26 = 0x6400000; //0x6400000
		Join_Channel_PlayerData_Response.unk27 = 0xC800000; //0xC800000
		Join_Channel_PlayerData_Response.unk28 = 0x19000000; //0x19000000
		Join_Channel_PlayerData_Response.unk29 = 0x32000000; //0x32000000
		Join_Channel_PlayerData_Response.unk30 = 0x64000000; //0x64000000
		Join_Channel_PlayerData_Response.unk31 = 0xC8000000; //0xC8000000
		Join_Channel_PlayerData_Response.unk32 = 0; //0
		Join_Channel_PlayerData_Response.unk33 = 0x90000000; //0x90000000
		Join_Channel_PlayerData_Response.unk34 = 1; //1
		Join_Channel_PlayerData_Response.unk35 = 0x20000000; //0x20000000
		Join_Channel_PlayerData_Response.unk36 = 3; //3
		Join_Channel_PlayerData_Response.unk37 = 0x40000000; //0x40000000
		Join_Channel_PlayerData_Response.unk38 = 6; //6
		Join_Channel_PlayerData_Response.ChannelType = 3; //3
		Join_Channel_PlayerData_Response.unk39 = 7; //7
		Join_Channel_PlayerData_Response.unk40 = 0x14; //0x14
		Join_Channel_PlayerData_Response.unk41 = 0; //0
		/*
		Join_Channel_PlayerData_Response.VisitBonusCode = 9999;
		Join_Channel_PlayerData_Response.VisitBonusElementType = 2;
		Join_Channel_PlayerData_Response.VisitBonusElementNumber = 5;
		Join_Channel_PlayerData_Response.VisitBonusElementMultiple = 1337;
		*/
		Join_Channel_PlayerData_Response.unk42; //0
		Join_Channel_PlayerData_Response.bunk[0] = 1; //1 0 1 1 1 1 0 0
		Join_Channel_PlayerData_Response.bunk[1] = 0;
		Join_Channel_PlayerData_Response.bunk[2] = 1;
		Join_Channel_PlayerData_Response.bunk[3] = 1;
		Join_Channel_PlayerData_Response.bunk[4] = 1;
		Join_Channel_PlayerData_Response.bunk[5] = 1;
		Join_Channel_PlayerData_Response.bunk[6] = 0;
		Join_Channel_PlayerData_Response.Rank = 101;
		Join_Channel_PlayerData_Response.unk43 = 1; //1
		Join_Channel_PlayerData_Response.unk44 = 22; //0x108
		memset((void*)&Join_Channel_PlayerData_Response.munk1,-1,4*8); //-1
		Join_Channel_PlayerData_Response.unk45 = 7; //7
		Join_Channel_PlayerData_Response.unk46 = 0x120101; //0x120101
		Join_Channel_PlayerData_Response.checksum = cIOSocket.MakeDigest((uint8*)&Join_Channel_PlayerData_Response);
		buffer = (unsigned char*)&Join_Channel_PlayerData_Response;
		break;
	case SHOP_JOIN_RESPONSE:
		Shop_Join_Response.size = 0x18;
		Shop_Join_Response.type = SHOP_JOIN_RESPONSE;
		Shop_Join_Response.unk1 = 11036;
		MySql.GetMoneyAmmount(Info.usr_id,&Shop_Join_Response.cash,0);
		Shop_Join_Response.state = UpdateState();
		Shop_Join_Response.checksum = cIOSocket.MakeDigest((uint8*)&Shop_Join_Response);
		buffer = (unsigned char*)&Shop_Join_Response;
		break;
	case SHOP_BUY_RESPONSE:
		memset(&Shop_Buy_Response,0,sizeof(ShopBuyResponse));
		Shop_Buy_Response.size = 0x698; //0x698
		Shop_Buy_Response.type = SHOP_BUY_RESPONSE;
		Shop_Buy_Response.unk1 = 11036; //11036
		Shop_Buy_Response.unk2 = 1; //1
		MySql.InsertNewItem(&Info,Shop_Buy_Request->item,Shop_Buy_Request->gf,Shop_Buy_Request->level,0);
		MySql.GetUserItems(Info.usr_id,Shop_Buy_Response.bMyCard, 0,Shop_Buy_Response.TypeMyCard, Shop_Buy_Response.GFMyCard,Shop_Buy_Response.LevelMyCard, Shop_Buy_Response.SkillMyCard);
		Shop_Buy_Response.Slots = Info.nSlots;
		{
			int codecost = 0,cashcost = 0;
			MySql.GetShopItemCost(Shop_Buy_Request->item,Shop_Buy_Request->gf,Shop_Buy_Request->level,codecost,cashcost);
			MySql.GetMoneyAmmount(Info.usr_id,(int*)&Shop_Buy_Response.Cash,&Info.Code,'-',cashcost,(unsigned __int64)codecost);
			Shop_Buy_Response.Code = Info.Code;
		}
		Shop_Buy_Response.state = UpdateState();
		Shop_Buy_Response.checksum = cIOSocket.MakeDigest((uint8*)&Shop_Buy_Response);
		buffer = (unsigned char*)&Shop_Buy_Response;
		break;
	case SHOP_SELL_RESPONSE:
		Shop_Sell_Response.size = 0x30;
		Shop_Sell_Response.type = SHOP_SELL_RESPONSE;
		Shop_Sell_Response.unk1 = 11036;
		Shop_Sell_Response.unk2 = 1;
		Shop_Sell_Response.item_slot = Shop_Sell_Request->item_slot;
		MySql.DeleteItem(Info.usr_id,Shop_Sell_Request->item_slot);
		Shop_Sell_Response.unk3 = 0;
		Shop_Sell_Response.Code = Info.Code + 2;
		Shop_Sell_Response.unk4 = 0;
		Shop_Sell_Response.state = UpdateState();
		Shop_Sell_Response.checksum = cIOSocket.MakeDigest((uint8*)&Shop_Sell_Response);
		buffer = (unsigned char*)&Shop_Sell_Response;
		break;
	case CARD_UPGRADE_RESPONSE:
		memset(&Card_Upgrade_Response,0,sizeof(Card_Upgrade_Response));
		Card_Upgrade_Response.size = 0x58;
		Card_Upgrade_Response.type = CARD_UPGRADE_RESPONSE;
		Card_Upgrade_Response.unk1 = 11036;
		Card_Upgrade_Response.Slot = Card_Upgrade_Request->Slot;
		Card_Upgrade_Response.UpgradeType = Card_Upgrade_Request->UpgradeType; //1 = Lvl, 2 = Skill
		Card_Upgrade_Response.unk2 = Card_Upgrade_Request->UpgradeType; //1 lvl, 5 skill
		Card_Upgrade_Response.UpgradeType2 = Card_Upgrade_Request->UpgradeType;
		MySql.UpgradeCard(&Info,&Card_Upgrade_Response);
		Card_Upgrade_Response.Code = Info.Code;
		if(Info.usr_room != -1)RoomList.ProdcastInRoomUpgrade(this,&Card_Upgrade_Response,Info.usr_room);
		Card_Upgrade_Response.state = UpdateState();
		Card_Upgrade_Response.checksum = cIOSocket.MakeDigest((uint8*)&Card_Upgrade_Response);
		buffer = (unsigned char*)&Card_Upgrade_Response;
		break;
	case LOBBY_USERINFO_RESPONSE:
		{
			int x = Lobby.GetList(pack);
			for(int i = 0; i < x; i++)
			{
				*(int*)(pack+(i*0x3C)+0x10) = UpdateState();
				*(int*)(pack+(i*0x3C)+0x0C) = cIOSocket.MakeDigest((uint8*)(pack+(i*0x3C)));
				Encrypt(pack+(i*0x3C));
			}
			nOfPackets = x*0x3C;
		}
		break;
	case USER_INFO_RESPONSE:
		memset(&User_Info_Response,0,sizeof(User_Info_Response));
		User_Info_Response.size = 0xF0;
		User_Info_Response.type = USER_INFO_RESPONSE;
		User_Info_Response.unk1 = 11036;
		User_Info_Response.bunk1 = 1; //online?
		strcpy(User_Info_Response.username,User_Info_Request->username);
		MySql.GetUserData(&User_Info_Response);
		User_Info_Response.state = UpdateState();
		User_Info_Response.checksum = cIOSocket.MakeDigest((uint8*)&User_Info_Response);
		buffer = (unsigned char*)&User_Info_Response;
		break;
	case ROOM_LIST_RESPONSE:
		memset(&Room_List_Response,0,sizeof(Room_List_Response));
		Room_List_Response.size = 0xC90;
		Room_List_Response.type = ROOM_LIST_RESPONSE;
		Room_List_Response.unk1 = 11036;
		Room_List_Response.unk4 = 0x500000;
		for(int i = 0; i < 22; i++)Room_List_Response.bunk[i] = true;
		RoomList.GetRoomList(&Room_List_Response);
		Room_List_Response.state = UpdateState();
		Room_List_Response.checksum = cIOSocket.MakeDigest((uint8*)&Room_List_Response);
		buffer = (unsigned char*)&Room_List_Response;
		break;
	case CHAT_RESPONSE:
		if(Chat_Request->chatType < 0 || Chat_Request->chatType > 7)break;
		if(Chat_Request->chatType == 1){
			if(strncmp(Chat_Request->recieverId, "console",8) == 0){
				console(Chat_Request->msg);
				MySql.InsertMsg(Chat_Request->senderId,"console",Chat_Request->msg);
			}
		}else {
		HandleList.ProdcastChat(this,Chat_Request);
		if(Chat_Request->chatType != 1)
		MySql.InsertMsg(Chat_Request->senderId,(char*)ChatTypes[Chat_Request->chatType],Chat_Request->msg);
		else MySql.InsertMsg(Chat_Request->senderId,Chat_Request->recieverId,Chat_Request->msg);
		}
		break;
	case ROOM_PLAYER_LIST_RESPONSE:
		memset(&Room_PlayerList_Response,0,sizeof(Room_PlayerList_Response));
		Room_PlayerList_Response.size = 0xCC;
		Room_PlayerList_Response.type = ROOM_PLAYER_LIST_RESPONSE;
		Room_PlayerList_Response.unk1 = 11036;
		RoomList.GetRoomPlayerList(Room_PlayerList_Request->roomnumber,&Room_PlayerList_Response);
		Room_PlayerList_Response.state = UpdateState();
		Room_PlayerList_Response.checksum = cIOSocket.MakeDigest((uint8*)&Room_PlayerList_Response);
		buffer = (unsigned char*)&Room_PlayerList_Response;
		break;
	case ROOM_JOIN_RESPONSE:
		Joined = true;
		if(GetRoomJoinResponse())
		RoomList.ProdcastRoomJoinResponse2(this,Info.usr_room);
		break;
	case ROOM_CREATE_RESPONSE:
		GetRoomCreateResponse();
		break;
	case ROOM_PLAYERDATA_RESPONSE:
		GetRoomPlayerData();
		nOfPackets = 0;
		/*
		nOfPackets = 1;
		After_Room_JoinResponse.size = 0x24;
		After_Room_JoinResponse.type = AFTER_ROOM_JOIN_RESPONSE;
		After_Room_JoinResponse.unk1 = 11036;
		After_Room_JoinResponse.unk2 = -1;
		After_Room_JoinResponse.unk3 = -1;
		After_Room_JoinResponse.unk4 = -1;
		After_Room_JoinResponse.checksum = cIOSocket.MakeDigest((uint8*)&After_Room_JoinResponse);
		buffer = (unsigned char*)&After_Room_JoinResponse;
		*/
		break;
	case CHANGE_ROOMTITLE_RESPONSE:
		if(RoomTitleChange_Request->room == Info.usr_room){
		RoomList.ProdcastChangeTitle(RoomTitleChange_Request);
		HandleList.ProdcastRoomUpdate(Info.usr_room);
		}
		nOfPackets = 0;
		break;
	case BUY_SCROLL_RESPONSE:
		MySql.GetScrolls(&Info);
		if(Scroll_BuyRequest->buy == 1){
			Info.scrolls[Scroll_BuyRequest->scrollid] = 0;
			MySql.UpdateScrolls(Info.usr_id,Scroll_BuyRequest->scrollid,0);
			BuyScroll_Response.unk2 = Scroll_BuyRequest->scrollid;
		}else {
			for(int i = 0; i < 3; i++)
				if(Info.scrolls[i] == 0){
					Info.scrolls[i] = Scroll_BuyRequest->scrollid;
					MySql.UpdateScrolls(Info.usr_id,i,Scroll_BuyRequest->scrollid+1);
					BuyScroll_Response.unk2 = i;
					break;
				}
		}
		GetRoomPlayerData();
		BuyScroll_Response.size = 0x30;
		BuyScroll_Response.type = BUY_SCROLL_RESPONSE;
		BuyScroll_Response.unk1 = 11036;
		//BuyScroll_Response.unk2 = 2;
		BuyScroll_Response.buy = Scroll_BuyRequest->buy;
		BuyScroll_Response.scrollid = Scroll_BuyRequest->scrollid+1;
		BuyScroll_Response.Code = Info.Code;
		BuyScroll_Response.zero1 = BuyScroll_Response.zero2 = 0;
		BuyScroll_Response.state = UpdateState();
		BuyScroll_Response.checksum = cIOSocket.MakeDigest((uint8*)&BuyScroll_Response);
		buffer = (unsigned char*)&BuyScroll_Response;
		break;
	case PICK_SCROLL_RESPONSE:
		PickScroll_Response.size = 0x20;
		PickScroll_Response.type = PICK_SCROLL_RESPONSE;
		PickScroll_Response.unk1 = 11036;
		PickScroll_Response.zero = PickScroll_Request->zero;
		PickScroll_Response.scroll = PickScroll_Request->scroll;
		PickScroll_Response.slot = PickScroll_Request->slot;
		MySql.UpdateScrolls(Info.usr_id,PickScroll_Response.slot,PickScroll_Response.scroll);
		PickScroll_Response.state = UpdateState();
		PickScroll_Response.checksum = cIOSocket.MakeDigest((uint8*)&PickScroll_Response);
		buffer = (unsigned char*)&PickScroll_Response;
		break;
	case SPAWN_RESPONSE:
		Spawn_Response.size = 0x38;
		Spawn_Response.type = SPAWN_RESPONSE;
		Spawn_Response.unk1 = 11036;
		for(int i = 0; i < 7; i++)
			Spawn_Response.slots[i] = -1;
		Spawn_Response.slots[Info.usr_slot] = 0;
		Spawn_Response.zero = 0;
		Spawn_Response.state = UpdateState();
		Spawn_Response.checksum = cIOSocket.MakeDigest((uint8*)&Spawn_Response);
		buffer = (unsigned char*)&Spawn_Response;
		break;
	case IN_GAME_RESPONSE:
		NewKing_Response.size = 0x18;
		NewKing_Response.type = IN_GAME_RESPONSE;
		NewKing_Response.unk1 = 11036;
		NewKing_Response.slot = 0; //King
		NewKing_Response.state = UpdateState();
		NewKing_Response.checksum = cIOSocket.MakeDigest((uint8*)&NewKing_Response);
		buffer = (unsigned char*)&NewKing_Response;
		break;
	case OPEN_USER_SHOP_REQ:
		OpenUserShop->state = UpdateState();
		OpenUserShop->checksum = cIOSocket.MakeDigest((uint8*)OpenUserShop);
		buffer = (unsigned char*)OpenUserShop;
		break;
	case CARD_SEARCH_RESPONSE:		
		CardSearch_Response.size = 0x108;
		CardSearch_Response.type = CARD_SEARCH_RESPONSE;
		CardSearch_Response.unk1 = 11036;
		MySql.SearchShop(&CardSearch_Response,CardSearch_Request->searchtype);
		CardSearch_Response.state = UpdateState();
		CardSearch_Response.checksum = cIOSocket.MakeDigest((uint8*)&CardSearch_Response);
		buffer = (unsigned char*)&CardSearch_Response;
		break;
	case MISSION_COMPLETE_RESPONSE:
		Info.Mission++;
		if(Info.Mission % 10 == 5)
			MySql.IncreaseMission(Info.usr_id,2);
		else MySql.IncreaseMission(Info.usr_id,1);
		MissionComplete.size = 0x1A0;
		MissionComplete.type = MISSION_COMPLETE_RESPONSE;
		MissionComplete.unk1 = 11036;
		MissionComplete.uk1[0]  =  1;
		for(int i = 1; i < 8; i++)MissionComplete.uk1[i] = -1;
		MissionComplete.level = 8; //maybe
		MissionComplete.zero1 = 0;
		MissionComplete.zero2 = 0;
		MissionComplete.Points = 1000; //maybe
		MissionComplete.Code = 3000; //maybe
		MissionComplete.dzero = 0;
		MissionComplete.dunk = 180;
		MissionComplete.dzero1 = 0;
		MissionComplete.dzero2 = 0;
		MissionComplete.dzero3 = 0;
		MissionComplete.dunk1 = 180;
		for(int i = 0; i < 9; i++)MissionComplete.dzeros[i] = 0;
		for(int i = 0; i < 8; i++)MissionComplete.ones[i] = 1;
		MissionComplete.unk2 = 10;
		MissionComplete.unk3 = 5;
		for(int i = 0; i < 7; i++)MissionComplete.ones2[i] = 1;
		MissionComplete.unk4 = 7;
		for(int i = 0; i < 19; i++)MissionComplete.unks[i] = -1;
		for(int i = 0; i < 8; i++)MissionComplete.zeros[i] = 0;
		for(int i = 0; i < 8; i++)MissionComplete.ones3[i] = 1;
		MissionComplete.unk5 = 268487332;
		MissionComplete.state = UpdateState();
		MissionComplete.checksum = cIOSocket.MakeDigest((uint8*)&MissionComplete);
		buffer = (unsigned char*)&MissionComplete;
		break;
	case MISSION_AFTER_RESPONSE:
		if(Info.Mission % 10 == 5)
			Info.Mission++;
		MissionAfter_Response.size = 0x1C;
		MissionAfter_Response.type  = MISSION_AFTER_RESPONSE;
		MissionAfter_Response.unk1 = 11036;
		MissionAfter_Response.unk2 = 3;
		MissionAfter_Response.unk3 = Info.Mission;
		MissionAfter_Response.state = UpdateState();
		MissionAfter_Response.checksum = cIOSocket.MakeDigest((uint8*)&MissionAfter_Response);
		buffer = (unsigned char*)&MissionAfter_Response;
		break;
	case NPC_LIST_RESPONSE:
		NpcList_Response.size = 0x38;
		NpcList_Response.type  = NPC_LIST_RESPONSE;
		NpcList_Response.unk1 = 11036;
		for(int i = 0; i < 33; i++)
			NpcList_Response.levels[i] = Npc_List->npc[i].level;
		NpcList_Response.state = UpdateState();
		NpcList_Response.checksum = cIOSocket.MakeDigest((uint8*)&NpcList_Response);
		buffer = (unsigned char*)&NpcList_Response;
		break;
	case NPC_KILL_RESPONSE:
		QuestGain_Response.size = 0xB0;
		QuestGain_Response.type = NPC_KILL_RESPONSE;
		QuestGain_Response.unk1 = 11036;
		QuestGain_Response.npc_num = NpcKill_Request->npc_num;
		QuestGain_Response.killerslot = Info.usr_slot;
		QuestGain_Response.zeros = 0;
		QuestGain_Response.timeStamp = NpcKill_Request->timeStamp;
		// Exp Calculation
		ExpCalc.Calculate(NpcKill_Request->npcType,&QuestGain_Response.exp,NpcKill_Request->damages);
		for(int i = 0; i < 16; i++)QuestGain_Response.unks[i] = -1;
		QuestGain_Response.eleType = (rand()%4)+1;
		QuestGain_Response.eleBase = (rand()%5)+1;
		QuestGain_Response.eleMul = (rand()%100)+1;
		RoomList.ProdcastExpGain(&QuestGain_Response,Info.usr_room);
        nOfPackets = 0;
		break;
		/*
		case QUEST_LIFE_RESPONSE:
		//{0x3c,0x0,0x0,0x0,0x56,0xe5,0xff,0xff,0x1f,0xa6,0xff,0xff,0xa0,0xec,0x22,0xc3,0x1c,0xd6,0x9b,0xf6,0xff,0xff,0xff,0xff,0xee,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
		{
		char unsigned respawn[] = {0xac,0x0,0x0,0x0,0x4e,0xe5,0xff,0xff,0x1f,0xa6,0xff,0xff,0xd8,0x6e,0x4e,0x64,0x5a,0xfc,0x2e,0x77,0xff,0xff,0xff,0xff,0xee,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xf7,0xff,0xff,0xff};
		Decrypt(respawn);

		*(int*)(respawn+0x10) = UpdateState();
		*(int*)(respawn+0xC)= cIOSocket.MakeDigest((uint8*)&respawn);
		Encrypt(respawn);
		memcpy(pack,respawn,sizeof(respawn));
		buffer = pack;
		nOfPackets = sizeof(respawn);
		}
		break;*/
	case BIGBATTLE_NPC_KO_RESPONSE:
		BigBattleNpcKo_Response.size = 0xA4;
		BigBattleNpcKo_Response.type = BIGBATTLE_NPC_KO_RESPONSE;
		BigBattleNpcKo_Response.unk1 = 11036;
		BigBattleNpcKo_Response.npcn = BigBattleNpcKo_Request->npcn;
		BigBattleNpcKo_Response.zero = 0;
		BigBattleNpcKo_Response.unk2 = 1;
		BigBattleNpcKo_Response.multiplier = 1;
		BigBattleNpcKo_Response.unk3 = 1;
		BigBattleNpcKo_Response.unk4 = 1;
		BigBattleNpcKo_Response.pointbase = 94;
		BigBattleNpcKo_Response.sub = 0xFFFFFFFA;
		for(int i = 0; i < 3; i++)BigBattleNpcKo_Response.zeros[i] = 0;
		BigBattleNpcKo_Response.npcn2 = BigBattleNpcKo_Request->npcn;
		for(int i = 0; i < 20; i++)BigBattleNpcKo_Response.unks[i] = -1;
		BigBattleNpcKo_Response.eleType = 1;
		BigBattleNpcKo_Response.eleBase = 1;
		BigBattleNpcKo_Response.eleMul = 2;
		BigBattleNpcKo_Response.one = 1;
		BigBattleNpcKo_Response.zero1 = 0;
		BigBattleNpcKo_Response.unk5 = -1;
		BigBattleNpcKo_Response.state = UpdateState();
		BigBattleNpcKo_Response.checksum = cIOSocket.MakeDigest((uint8*)&BigBattleNpcKo_Response);
		buffer = (unsigned char*)&BigBattleNpcKo_Response;
		break;
	case REVIVE_RESPONSE:
		RoomList.ProdcastReviveResponse(Revive_Response,Info.usr_room);
		nOfPackets = 0;
		break;
	case PLAYER_KICK_RESPONSE:
		if(Info.usr_slot == Info.rm_master) 
		RoomList.ProdcastKickResponse(Info.usr_room,PlayerKick_Request->slot);
		nOfPackets = 0;
		break;
	case ROOM_EXIT_RESPONSE:
		GetExitRoomResponse();
		nOfPackets = 0;
		break;
	}
}

char* PacketHandler::ServerResponse()
{
	Encrypt(buffer);
	return (char*)buffer;
}

int PacketHandler::ServerResponse(unsigned char* buf)
{
	if (nOfPackets == 1)
	{
		Encrypt(buffer);
		memcpy(buf,buffer,(*(int*)buffer));
	}
	else if (nOfPackets > 1)
	{
		memcpy(buf,pack,nOfPackets);
	}

	return nOfPackets;
}


void PacketHandler::SetClientIP(char *ip)
{
	strcpy(IP,ip);
}
