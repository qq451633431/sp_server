#include "UserList.h"
IOSocket cIOSocket;
LobbyList Lobby;

void PacketHandler::GetUserInfo()
{
    MySql.GetUserInfo(Join_Channel_Request->username,Info);
    LobbyInfo.name = Join_Channel_Request->username;
    LobbyInfo.level = Info.Level;
    LobbyInfo.mission = Info.Mission;
    LobbyInfo.gender = Info.gender;
	MySql.GetScrolls(&Info);
}

void PacketHandler::GetUserItems()
{
    MySql.GetUserItems(Info.usr_id,Join_Channel_PlayerData_Response.bMyCard, Join_Channel_PlayerData_Response.IDMyCard,
                       Join_Channel_PlayerData_Response.TypeMyCard, Join_Channel_PlayerData_Response.GFMyCard,
                       Join_Channel_PlayerData_Response.LevelMyCard, Join_Channel_PlayerData_Response.SkillMyCard);
	MySql.GetScrolls(&Info);
}

void PacketHandler::GetChatMessage(ChatResponse *Chat_Response)
{
	if(Chat_Response->chatType == 0 || Chat_Response->chatType == 7)
		if(Info.usr_room != -1)return;
	if(Chat_Response->chatType == 1)
		if(strcmp(Chat_Response->recieverId, Info.usr_name) != 0)return;
    Chat_Response->state = UpdateState();
    Chat_Response->checksum = cIOSocket.MakeDigest((uint8*)Chat_Response);
    unsigned char msg[0x80];
    memcpy(msg,Chat_Response,0x7C);
    //Encrypt(msg);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetLobbyMessage(LobbyUserInfoResponse* LUIR)
{
    if(Info.usr_room != -1)return;

    LUIR->state = UpdateState();
    LUIR->checksum = cIOSocket.MakeDigest((uint8*)LUIR);
    unsigned char msg[0x40];
    memcpy(msg,LUIR,0x3C);
    //Encrypt((unsigned char*)&msg);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));

    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetNewRoomMessage(LobbyRoomResponse* LRR)
{
    LRR->state = UpdateState();
    LRR->checksum = cIOSocket.MakeDigest((uint8*)LRR);
    unsigned char msg[0xB4];
    memcpy(msg,LRR,0xB0);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));

    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetInRoomPlayerList(RoomPlayerDataResponse* RPDR)
{
    RPDR->state = UpdateState();
    RPDR->checksum = cIOSocket.MakeDigest((uint8*)RPDR);
    unsigned char msg[0x120];
    memcpy(msg,RPDR,0x120);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));

    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetExitRoomResponse()
{
    memset(&Room_Exit_Response,0,sizeof(RoomExitResponse));
    Room_Exit_Response.size = 0x28;
    Room_Exit_Response.type = ROOM_EXIT_RESPONSE;
    Room_Exit_Response.unk1 = 11036;
    Room_Exit_Response.unk2 = Info.usr_slot;
    strcpy(Room_Exit_Response.username,LobbyInfo.name.c_str());
    RoomList.ProdcastPlayerExitRoom(this,&Room_Exit_Response,Info.usr_room);
    Room_Exit_Response.state = UpdateState();
    Room_Exit_Response.checksum = cIOSocket.MakeDigest((uint8*)&Room_Exit_Response);
    buffer = (unsigned char*)&Room_Exit_Response;
    for (int i = 4; i < *(int*)buffer; i++)
        buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
    send(msg_socket,(char*)buffer,*(int*)buffer, 0);
    if(RoomList.ExitPlayer(Info.usr_room,this))
    {
        HandleList.ProdcastNewRoom(this,0,false,Info.usr_room);
    }
	LastPosition = 0;
    Info.usr_char = Info.DefaultCharacter + Info.premium;
	Info.usr_room = -1;
    Lobby.Insert(LobbyInfo);
    HandleList.ProdcastLobbyInfo(this,&LobbyInfo,true);
    GenerateResponse(LOBBY_USERINFO_RESPONSE);
    send(msg_socket,(char*)pack,nOfPackets, 0);
    //HandleList.ProdcastRoomUpdate(Info.usr_room);
	GetRoomListResponse();
}

void PacketHandler::GetPlayerExitRoomResponse(RoomExitResponse *RER)
{
    RER->state = UpdateState();
    RER->checksum = cIOSocket.MakeDigest((uint8*)RER);
    unsigned char msg[0x2C];
    memcpy(msg,RER,0x2C);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));

    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetInRoomData(RoomPlayerDataResponse* RPDR, bool started)
{
    RPDR->Character = Info.usr_char;
    strcpy(RPDR->netip,IP);
    if(!LocIP[0])
    {
        MySql.GetUserIP(Info.usr_id,LocIP);
    }
    strcpy(RPDR->locip,LocIP);
    RPDR->level = Info.Level;
    strcpy(RPDR->username,LobbyInfo.name.c_str());
    RPDR->missionlevel = Info.Mission;
    MySql.GetEquipData(Info.usr_id,RPDR);
    RPDR->Ready = Info.usr_ready;
    //RPDR->team = Info.usr_team;
	RPDR->unk4 = 1;
	RPDR->unk9 = 0x01FFFFFF;
	RPDR->team = Info.usr_team;
    if(Info.usr_mode == 5)
        switch (Info.usr_slot)
        {
        case 0:
            RPDR->team = 10;
            break;
        case 1:
            RPDR->team = 20;
            break;
        }

	if(started && Info.usr_ready && !Joined)
    {
        RPDR->Start = 0;
		RPDR->bunk1 = false; //0 = GameStart
		if(Info.rm_master == Info.usr_slot)RoomList.InitializeLife(Info.usr_room);
    }
	else {
        RPDR->Start = 2;
		RPDR->bunk1 = false; //0 = GameStart
	}
	if(Info.rm_master != Info.usr_slot)
		RPDR->bunk1 = true;
    //RPDR->unk22 = 0x01010101;
	RPDR->bunk2 = Info.questlife; //QuestLife
	RPDR->bunk3 = 1;
	RPDR->bunk4 = 0; 
	RPDR->scroll[0] = Info.scrolls[0];
    RPDR->scroll[1] = Info.scrolls[1];
    RPDR->scroll[2] = Info.scrolls[2];
}

void PacketHandler::GetInRoomUpgradeResponse(CardUpgradeResponse *CUR)
{
    CUR->state = UpdateState();
    CUR->checksum = cIOSocket.MakeDigest((uint8*)CUR);
    unsigned char msg[0x5C];
    memcpy(msg,CUR,0x5C);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));

    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetRoomListResponse()
{
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
    for (int i = 4; i < *(int*)buffer; i++)
        buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
    send(msg_socket,(char*)buffer,*(int*)buffer, 0);
}

bool PacketHandler::GetRoomJoinResponse()
{
    memset(&Room_Join_Response,0,sizeof(Room_Join_Response));
    Room_Join_Response.size = 0x7C;
    Room_Join_Response.type = ROOM_JOIN_RESPONSE;
    Room_Join_Response.unk1 = 11036;
    Room_Join_Response.roomnumber = Room_Join_Request->roomnumber;
    Info.usr_room = Room_Join_Request->roomnumber;
    bool join = RoomList.JoinRoom(this,&LobbyInfo.name,Room_Join_Request->roomnumber,Info.gender,Info.Level);
    if(join)RoomList.GetRoomData(&Room_Join_Response);
	Room_Join_Response.unk2 = !join;
    Room_Join_Response.unk05 = 1; //1
    Room_Join_Response.Slot = Info.usr_slot; //1
    Room_Join_Response.unk09 = 1; //0xA
    Room_Join_Response.unk11 = -1;
    Room_Join_Response.unk13 = 1;
    Room_Join_Response.state = UpdateState();
    Room_Join_Response.checksum = cIOSocket.MakeDigest((uint8*)&Room_Join_Response);
    buffer = (unsigned char*)&Room_Join_Response;
    if(join)Lobby.Delete(LobbyInfo.name);
    if(join)HandleList.ProdcastLobbyInfo(this,&LobbyInfo,false);
	if(join)HandleList.ProdcastRoomUpdate(Info.usr_room);
    for (int i = 4; i < *(int*)buffer; i++)
        buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
    send(msg_socket,(char*)buffer,*(int*)buffer, 0);
    //nOfPackets = 0;
    if(join)GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
	return join;
}

void PacketHandler::GetRoomCreateResponse()
{
    memset(&Create_Room_Response.size,0,sizeof(CreateRoomResponse));
    Create_Room_Response.size = 0x6C;
    Create_Room_Response.type = ROOM_CREATE_RESPONSE;
    Create_Room_Response.unk1 = 11036;
    Create_Room_Response.state = UpdateState();
    Create_Room_Response.unk01 = 2; //CreateRoom, 0 = no room to create
    Create_Room_Response.roomnumber = Create_Room_Request->roomnumber;
    Info.usr_room = Create_Room_Request->roomnumber;
    strcpy(Create_Room_Response.roomname,Create_Room_Request->roomname);
    Create_Room_Response.mode = Create_Room_Request->mode; //0x1b
    Create_Room_Response.map = Create_Room_Request->map; //0x1b
    strcpy(Create_Room_Response.password,Create_Room_Request->password);
    Create_Room_Response.capacity = Create_Room_Request->capacity;
    Create_Room_Response.allowscrolls = 1; //1
    Create_Room_Response.autoteam = 1; //1
    Create_Room_Response.unk2 = 12;
    Create_Room_Response.character = Info.DefaultCharacter+120; //0x0A
    Create_Room_Response.unk03 = 0x74F59300; //0x74F59300
    Create_Room_Response.maxcardlevel = -1;
    Create_Room_Response.allowcritsheild = -1;
    Create_Room_Response.unk3 = -1; //0
    Create_Room_Response.unk4 = 1; //0
    Create_Room_Response.unk5 = 0; //0
    Create_Room_Response.checksum = cIOSocket.MakeDigest((uint8*)&Create_Room_Response);
    buffer = (unsigned char*)&Create_Room_Response;
    RoomList.CreateRoom(this,&LobbyInfo.name,&Create_Room_Response,Info.gender,Info.Level);
    HandleList.ProdcastNewRoom(this,&Create_Room_Response,true);
    Lobby.Delete(LobbyInfo.name);
    HandleList.ProdcastLobbyInfo(this,&LobbyInfo,false);
    for (int i = 4; i < *(int*)buffer; i++)
        buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
    send(msg_socket,(char*)buffer,*(int*)buffer, 0);
    GenerateResponse(ROOM_PLAYERDATA_RESPONSE);
}

void PacketHandler::GetInRoomDeathResponse(PlayerKilledResponse *PKR)
{
    int Points = 200, Killer = PKR->KillerSlot;
    /*
    if(Info.usr_slot == PKR->DeadplayerSlot)
    {
        PKR->GainedPoints = -Points;
        PKR->LostPoints = Points;
    }
    else
    {*/
    PKR->GainedPoints = -Points;
    PKR->LostPoints = Points;
    //}
    PKR->state = UpdateState();
    PKR->checksum = cIOSocket.MakeDigest((uint8*)PKR);
    unsigned char msg[0xB0];
    memcpy(msg,PKR,0xB0);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
    if(Info.usr_mode == 32)
    {
        NewKing_Response.size = 0x18;
        NewKing_Response.type = IN_GAME_RESPONSE;
        NewKing_Response.unk1 = 11036;
        NewKing_Response.slot = Killer;
        NewKing_Response.state = UpdateState();
        NewKing_Response.checksum = cIOSocket.MakeDigest((uint8*)&NewKing_Response);
        buffer = (unsigned char*)&NewKing_Response;
        for (int i = 4; i < *(int*)buffer; i++)
            buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
        send(msg_socket,(char*)buffer,*(int*)buffer, 0);
        if(Info.usr_slot != Killer)
        {
            RoundClear_Response.size = 0x38;
            RoundClear_Response.type = ROUND_CLEAR_RESPONSE;
            RoundClear_Response.unk1 = 11036;
            RoundClear_Response.Slot = 0; //maybe
            RoundClear_Response.Points = Info.Points;
            RoundClear_Response.Code = Info.Code;
            RoundClear_Response.unk2 = 0;
            RoundClear_Response.unk3 = 1;
            RoundClear_Response.state = UpdateState();
            RoundClear_Response.checksum = cIOSocket.MakeDigest((uint8*)&RoundClear_Response);
            buffer = (unsigned char*)&RoundClear_Response;
            for (int i = 4; i < *(int*)buffer; i++)
                buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
            send(msg_socket,(char*)buffer,*(int*)buffer, 0);
        }
    }
}

void PacketHandler::Trader()
{
    RoomList.SendTradeResponse(&Trade_Struct,Info.usr_room);
    Trade_Struct.state = UpdateState();
    Trade_Struct.checksum = cIOSocket.MakeDigest((uint8*)&Trade_Struct);
    buffer = (unsigned char*)&Trade_Struct;
    for (int i = 4; i < *(int*)buffer; i++)
        buffer[i] = ~((BYTE)(buffer[i] << 3) | (BYTE)(buffer[i] >> 5));
    send(msg_socket,(char*)buffer,*(int*)buffer, 0);
}

void PacketHandler::GetTradeResponse(TradeStruct *TS)
{
    TS->state = UpdateState();
    TS->checksum = cIOSocket.MakeDigest((uint8*)TS);
    unsigned char msg[0x9C];
    memcpy(msg,TS,0x9C);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetRoomPlayerData()
{
	if(Joined)
    {
        GetMasterResponse(Info.rm_master);
        Joined = false;
    }
    int x = RoomList.GetInRoomPlayerList(Info.usr_room,pack);
    for(int i = 0; i < x; i++)
    {
        *(int*)(pack+(i*0x118)+0x10) = UpdateState();
        *(int*)(pack+(i*0x118)+0x0C) = cIOSocket.MakeDigest((uint8*)(pack+(i*0x118)));
        Encrypt(pack+(i*0x118));
    }
    send(msg_socket,(char*)pack,x*0x118, 0);
    /*NpcList myList;
    RoomList.GetNpcList(&myList,Info.usr_room);
    if(Info.usr_ready)
        if(myList.size == 0x404)
        {
            myList.state = UpdateState();
            myList.checksum = cIOSocket.MakeDigest((uint8*)&myList);
            unsigned char *msg = (unsigned char*)&myList;
            for (int i = 4; i < *(int*)msg; i++)
                msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
            send(msg_socket,(char*)msg,*(int*)msg, 0);
        }
		*/
}

void PacketHandler::GetMasterResponse(int mSlot)
{
	Info.rm_master = mSlot;
    NewMasterResponse NMR;
    NMR.size = 0x1C;
    NMR.type = NEW_MASTER_RESPONSE;
    NMR.unk1 = 11036;
    NMR.Slot = mSlot;
    NMR.unk2 = 1;
    NMR.state = UpdateState();
    NMR.checksum = cIOSocket.MakeDigest((uint8*)&NMR);
    unsigned char *msg = (unsigned char*)&NMR;
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetNpcList(NpcList *npc)
{
	if(!Info.usr_ready)return;
	npc->state = UpdateState();
	npc->checksum = cIOSocket.MakeDigest((uint8*)npc);
	unsigned char msg[0x408];
    memcpy(msg,npc,0x408);
	for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetExpGainResponse(QuestGainResponse *QGR)
{
	if(Info.usr_slot >= 0 && Info.usr_slot < 8)
	{
		if(QGR->killerslot == Info.usr_slot && QGR->eleType > 0 && QGR->eleType < 5)
			MySql.GetExp(Info.usr_id,QGR->exp.exp[Info.usr_slot]*QGR->exp.cpMul[Info.usr_slot],ElementTypes[QGR->eleType],QGR->eleBase*QGR->eleMul);
		else MySql.GetExp(Info.usr_id,QGR->exp.exp[Info.usr_slot]*QGR->exp.cpMul[Info.usr_slot]);
	}
	QGR->state = UpdateState();
	QGR->checksum = cIOSocket.MakeDigest((uint8*)QGR);
    unsigned char msg[0xB4];
    memcpy(msg,QGR,0xB4);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetReviveResponse(ReviveResponse *Revive_Response)
{
    Revive_Response->state = UpdateState();
    Revive_Response->checksum = cIOSocket.MakeDigest((uint8*)Revive_Response);
    unsigned char msg[0x24];
    memcpy(msg,Revive_Response,0x24);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetResultResponse(ResultsResponse* Results_Response)
{
	Info.usr_ready = 0;
	Results_Response->Points = Info.Points;
	Results_Response->Code = Info.Code;
    Results_Response->state = UpdateState();
    Results_Response->checksum = cIOSocket.MakeDigest((uint8*)Results_Response);
    unsigned char msg[0x94];
    memcpy(msg,Results_Response,0x94);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetBigBattleNpcMultiplier()
{
	if(Info.usr_mode != 0x21)return;
    BigBattleNpcMultiplier BBNM;
    BBNM.size = 0x38;
    BBNM.type = BIGBATTLE_NPC_X_RESPONSE;
    BBNM.unk1 = 11036;
	BBNM.x[0] = 2;
	BBNM.x[1] = 5;
	BBNM.x[2] = 10;
	for(int i = 3; i < 36; i++)
		BBNM.x[i] = 1;	
    BBNM.state = UpdateState();
    BBNM.checksum = cIOSocket.MakeDigest((uint8*)&BBNM);
    unsigned char *msg = (unsigned char*)&BBNM;
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetJoinResponse2()
{
	RoomJoinResponse2 RJR2;
	RJR2.size = 0x24;
    RJR2.type = ROOM_JOIN_RESPONSE2;
    RJR2.unk1 = 11036;
	for(int i = 0; i < 4; i++)
		RJR2.unks[i] = -1;
	RJR2.state = UpdateState();
    RJR2.checksum = cIOSocket.MakeDigest((uint8*)&RJR2);
    unsigned char *msg = (unsigned char*)&RJR2;
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

void PacketHandler::GetKickResponse(int slot)
{
	PlayerKickResponse PKR;
    PKR.size = 0x18;
    PKR.type = PLAYER_KICK_RESPONSE;
    PKR.unk1 = 11036;
	PKR.slot = slot;
	PKR.state = UpdateState();
    PKR.checksum = cIOSocket.MakeDigest((uint8*)&PKR);
    unsigned char *msg = (unsigned char*)&PKR;
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
	if(Info.usr_slot == slot){
		if(RoomList.ExitPlayer(Info.usr_room,this))
			HandleList.ProdcastNewRoom(this,0,false,Info.usr_room);
		Info.usr_char = Info.DefaultCharacter + Info.premium;
		Lobby.Insert(LobbyInfo);
		HandleList.ProdcastLobbyInfo(this,&LobbyInfo,true);
		GenerateResponse(LOBBY_USERINFO_RESPONSE);
		send(msg_socket,(char*)pack,nOfPackets, 0);
		HandleList.ProdcastRoomUpdate(Info.usr_room);
		Info.usr_room = -1;
	}
}

void PacketHandler::GetTitleChange(RoomTitleChangeResponse *RTCR)
{
	RTCR->state = UpdateState();
    RTCR->checksum = cIOSocket.MakeDigest((uint8*)RTCR);
    unsigned char msg[0x38];
    memcpy(msg,RTCR,0x34);
    for (int i = 4; i < *(int*)msg; i++)
        msg[i] = ~((BYTE)(msg[i] << 3) | (BYTE)(msg[i] >> 5));
    send(msg_socket,(char*)msg,*(int*)msg, 0);
}

//link to udp
void udpGetUserName(char *name,int room,int slot)
{
	RoomList.GetUserName(name,room,slot);
}