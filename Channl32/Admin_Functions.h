void PacketHandler::console(char *cmd){
	if(cmd[0] == '@'){
		int len = strlen(cmd);
		for(int i = 0; i < len; i++)
			cmd[i] = tolower(cmd[i]);
		if(strncmp(cmd+1,"gm",2) == 0)SendGmMessage(cmd+4);
		else if(strncmp(cmd+1,"dc",2) == 0)DisconnectPlayer(cmd+4);
		else if(strncmp(cmd+1,"kick",4) == 0)KickPlayer(cmd+6);
		else if(strncmp(cmd+1,"revive",4) == 0)RevivePlayer(cmd+6);
	}
}

void PacketHandler::SendGmMessage(char *text)
{
	Chat_Request->chatType = 6;
	strcpy(Chat_Request->msg,text);
	MySql.InsertMsg(Chat_Request->senderId,"GM",Chat_Request->msg);
	for(int i = 0; i < 13; i++)
		Chat_Request->senderId[i] = Chat_Request->recieverId[i] = 0;
	HandleList.ProdcastChat(0,Chat_Request);
}

void PacketHandler::DisconnectPlayer(char *name)
{
	//not that important
}

void PacketHandler::KickPlayer(char *cmd)
{
	char room[3], slot[3];
	short nroom = -1, nslot = -1;
	for(int i = 0; i < 3; i++)room[i] = slot[i] = 0;
	int len = strlen(cmd);
	int i = 0;
	for(int a = 0; a < 3; i++, a++)
		if(cmd[i] == ' ' || i >= len)break;
		else room[a] = cmd[i];
	while(cmd[i] == ' ' && i < (len-3))i++;
	for(int a = 0; a < 3; i++, a++)
		if(cmd[i] == ' ' || i >= len)break;
		else slot[a] = cmd[i];
	if(isdigit(slot[0]))nslot = atoi(slot);
	else if(strncmp(slot,"all",3) == 0)nslot = 100;
	if(isdigit(room[0]))nroom = atoi(room)-1;
	else if(room[0] == '.')nroom = Info.usr_room;
	if(nroom == -1)return;
	if(nslot >= 0 && nslot < 8)
		RoomList.ProdcastKickResponse(nroom,nslot);
	else if(nslot == 100){
		for(int i = 0; i < 8; i++)
			RoomList.ProdcastKickResponse(nroom,i);
	}
}

void PacketHandler::RevivePlayer(char *cmd)
{
	ReviveResponse RR;
    RR.size = 0x20;
    RR.unk1 = 11036;
    RR.unk2 = 0;
    RR.unk3 = 0;
	for(int i = 0; i < 8; i++){
	RR.Slot = i;
	RoomList.ProdcastReviveResponse(&RR,Info.usr_room);
	}
}