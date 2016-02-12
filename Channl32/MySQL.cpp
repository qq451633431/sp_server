#include "MySQL.h"
#include "ini.h"

extern CIni config;

MySQL::MySQL()
{
	config.SetSection("DB");
	char * ip = config.ReadString("ip", "178.162.181.68");
	uint32 port = config.ReadInteger("port", 3306);
	char * user = config.ReadString("user", "sp");
	char * pw = config.ReadString("pw", "pserver1");
	char * db = config.ReadString("db", "sp_db");

	connection = mysql_init(0);
	if(!mysql_real_connect(connection, ip, user, pw, db, port, 0, 0))
        printf("Unable to connect to MySQL server\n");
}

MySQL::~MySQL()
{
	mysql_close(connection);
}

void MySQL::SetUserIP(char *name,char* ip)
{
    char buffer[200];
    sprintf(buffer,"UPDATE users SET usr_ip = \'%s\' WHERE usr_name = \'%s\'",ip,name);
    mysql_query(connection,buffer);
}

void MySQL::GetUserIP(int id,char *ip)
{
    char buffer[200];
    sprintf(buffer,"SELECT usr_ip FROM users WHERE usr_id = %d",id);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    strcpy(ip,result[0]);
    mysql_free_result(res);
}

void MySQL::GetUserInfo(char *name, MyCharInfo &info)
{
    char buffer[300];
    sprintf(buffer,"SELECT usr_id, usr_gender, usr_char, usr_points, usr_code, usr_level, usr_mission, usr_nslots, usr_water, usr_fire, usr_earth, usr_wind, usr_coins FROM users WHERE usr_name = \'%s\'",name);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    info.usr_id = atoi(result[0]);
    info.gender = atoi(result[1]);
    info.DefaultCharacter = atoi(result[2]);
    info.Points = _atoi64(result[3]);
    info.Code = _atoi64(result[4]);
    info.Level = atoi(result[5]);
    info.Mission = atoi(result[6]);
    info.nSlots = atoi(result[7]);
    info.UserType = 30;
    info.Water = atoi(result[8]);
    info.Fire = atoi(result[9]);
    info.Earth = atoi(result[10]);
    info.Wind = atoi(result[11]);
    info.Coins = atoi(result[12]);
	strcpy(info.usr_name,name);
    mysql_free_result(res);
}

void MySQL::GetUserItems(int id,bool *bMyCard,int *IDMyCard,int *TypeMyCard,int *GFMyCard,int *LevelMyCard,int *SkillMyCard)
{
    char buffer[200];
    sprintf(buffer,"SELECT itm_id, itm_slot, itm_type, itm_gf, itm_level, itm_skill FROM items WHERE itm_usr_id = %d",id);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result;
    while(result = mysql_fetch_row(res))
    {
        int slot = atoi(result[1]);
        bMyCard[slot] = true;
        if(IDMyCard)IDMyCard[slot] = atoi(result[0]);
        TypeMyCard[slot] = atoi(result[2]);
        GFMyCard[slot] = atoi(result[3]);
        LevelMyCard[slot] = atoi(result[4]);
        SkillMyCard[slot] = atoi(result[5]);
    }
    mysql_free_result(res);
}

int MySQL::GetValidSlot(int id,int maxslots)
{
    char buffer[200];
    sprintf(buffer,"SELECT itm_slot FROM items WHERE itm_usr_id = %d",id);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result;
    for(int i = 0; i < maxslots; i++)
        buffer[i] = i;
    while(result = mysql_fetch_row(res))
    {
        int n = atoi(result[0]);
        buffer[n] = -1;
    }
    mysql_free_result(res);
    for(int i = 0; i < maxslots; i++)
        if(buffer[i] != -1)return buffer[i];
    return -1;
}
void MySQL::InsertNewItem(MyCharInfo *Info,int item,int gf,int level,int skill)
{
    int validslot = GetValidSlot(Info->usr_id,Info->nSlots);
    if(validslot == -1)return;
    char buffer[200];
    sprintf(buffer,"INSERT INTO items VALUES (0,%d,%d,%d,%d,%d,%d)",validslot,Info->usr_id,item,gf,level,skill);
    mysql_query(connection,buffer);
}

int MySQL::DeleteItem(int id,int slotn)
{
    char buffer[200];
    sprintf(buffer,"SELECT itm_type FROM items WHERE itm_usr_id = %d AND itm_slot = %d",id,slotn);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return 0;
    }
    int itm_type = atoi(result[0]);
    mysql_free_result(res);
    sprintf(buffer,"DELETE FROM items WHERE itm_usr_id = %d AND itm_slot = %d",id,slotn);
    mysql_query(connection,buffer);
    return itm_type;
}

void MySQL::GetShopItemCost(int item, int gf,int quantity,int &code,int &cash)
{
    char buffer[250];
    sprintf(buffer,"SELECT itm_gf, itm_code_cost, itm_cash_cost, itm_coins_cost FROM shop WHERE itm_type = %d AND itm_quantity = %d",item,quantity);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result;
    while(result = mysql_fetch_row(res))
    {
        int x = atoi(result[0]);
        if(gf == x || (!gf && !x))
        {
            cash = atoi(result[2]);
            break;
        }
        else if(!gf && x)
        {
            code = atoi(result[1]);
            break;
        }
    }
    mysql_free_result(res);

}

void MySQL::InsertMsg(char *sender,char *reciever,char *msg)
{
    char buffer[200];
    sprintf(buffer,"INSERT INTO chats VALUES (0,1,\'%s\',\'%s\',\'%s\')",sender,reciever,msg);
    mysql_query(connection,buffer);
}

void MySQL::ChangeEquips(int id,EquipChangeRequest *ECR)
{
    if(!ECR)return;
    char buffer[400];
    sprintf(buffer,"UPDATE equipments SET eqp_mag = %d, eqp_wpn = %d, eqp_arm = %d, eqp_pet = %d, eqp_foot = %d, eqp_body = %d, eqp_hand1 = %d, eqp_hand2 = %d, eqp_face = %d, eqp_hair = %d, eqp_head = %d WHERE usr_id = %d",ECR->mag,ECR->wpn,ECR->arm,ECR->pet,ECR->foot,ECR->body,ECR->hand1,ECR->hand2,ECR->face,ECR->hair,ECR->head,id);
    mysql_query(connection,buffer);
}

void MySQL::GetUserData(UserInfoResponse* UIR)
{
    char buffer[500];
    sprintf(buffer,"SELECT usr_id, usr_gender, usr_char, usr_points, usr_code, usr_coins, usr_level, usr_mission, usr_wins, usr_losses, usr_ko, usr_down FROM users WHERE usr_name = \'%s\'",UIR->username);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    int uid = atoi(result[0]);
    UIR->gender = atoi(result[1]);
    UIR->defaultcharacter = atoi(result[2]);
    UIR->Points = _atoi64(result[3]);
    UIR->Codes = _atoi64(result[4]);
    UIR->Coins = atoi(result[5]);
    UIR->level = atoi(result[6]);
    UIR->mission = atoi(result[7]);
    UIR->wins = atoi(result[8]);
    UIR->losses = atoi(result[9]);
    UIR->KO = atoi(result[10]);
    UIR->Down = atoi(result[11]);

    mysql_free_result(res);

    sprintf(buffer,"SELECT itm_type, itm_gf, itm_level, itm_skill FROM equipments INNER JOIN items WHERE itm_usr_id = %d AND usr_id = %d AND (itm_slot = eqp_mag OR itm_slot = eqp_wpn OR itm_slot = eqp_arm OR itm_slot = eqp_pet OR itm_slot = eqp_foot OR itm_slot = eqp_body OR itm_slot = eqp_hand1 OR itm_slot = eqp_hand2 OR itm_slot = eqp_face OR itm_slot = eqp_hair OR itm_slot = eqp_head)",uid,uid);
    mysql_query(connection,buffer);
    res = mysql_use_result(connection);
    ItemId Item_id;
    while(result = mysql_fetch_row(res))
    {
        int itm_type = atoi(result[0]), itm_gf = atoi(result[1]), itm_level = atoi(result[2]), itm_skill = atoi(result[3]);
        int id1 = Item_id.Identify(itm_type);
        if(id1 == black || id1 == gold || id1 == pet)
        {
            switch (Item_id.IdentifyItem(itm_type))
            {
            case ct_mag:
                UIR->magic = itm_type;
                UIR->magicgf = itm_gf;
                UIR->magiclevel = itm_level;
                UIR->magicskill = itm_skill;
                break;
            case ct_wpn:
                UIR->weapon = itm_type;
                UIR->weapongf = itm_gf;
                UIR->weaponlevel = itm_level;
                UIR->weaponskill = itm_skill;
                break;
            case ct_arm:
                UIR->armour = itm_type;
                UIR->armourgf = itm_gf;
                UIR->armourlevel = itm_level;
                UIR->armourskill = itm_skill;
                break;
            case ct_pet:
                UIR->pet = itm_type;
                UIR->petgf = itm_gf;
                UIR->petlevel = itm_level;
                break;
            default:
                break;
            }
        }
        else if(id1 == avatar)
        {
            switch (Item_id.IdentifyItem(itm_type))
            {
            case ct_foot:
                UIR->foot = itm_type;
                break;
            case ct_body:
                UIR->body = itm_type;
                break;
            case ct_hand1:
                UIR->hand1 = itm_type;
                break;
            case ct_hand2:
                UIR->hand2 = itm_type;
                break;
            case ct_face:
                UIR->face = itm_type;
                break;
            case ct_hair:
                UIR->hair = itm_type;
                break;
            case ct_head:
                UIR->head = itm_type;
                break;
            default:
                break;
            }
        }
    }
    mysql_free_result(res);
}

void MySQL::GetEquipData(int id, RoomPlayerDataResponse *RPDR)
{
    char buffer[500];
    sprintf(buffer,"SELECT itm_type, itm_gf, itm_level, itm_skill FROM equipments INNER JOIN items WHERE itm_usr_id = %d AND usr_id = %d AND (itm_slot = eqp_mag OR itm_slot = eqp_wpn OR itm_slot = eqp_arm OR itm_slot = eqp_pet OR itm_slot = eqp_foot OR itm_slot = eqp_body OR itm_slot = eqp_hand1 OR itm_slot = eqp_hand2 OR itm_slot = eqp_face OR itm_slot = eqp_hair OR itm_slot = eqp_head)",id,id);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result;
    ItemId Item_id;
    while(result = mysql_fetch_row(res))
    {
        int itm_type = atoi(result[0]), itm_gf = atoi(result[1]), itm_level = atoi(result[2]), itm_skill = atoi(result[3]);
        int id1 = Item_id.Identify(itm_type);
        if(id1 == black || id1 == gold || id1 == pet)
        {
            switch (Item_id.IdentifyItem(itm_type))
            {
            case ct_mag:
                RPDR->magictype = itm_type;
                RPDR->magicgf = itm_gf;
                RPDR->magiclevel = itm_level;
                RPDR->magicskill = itm_skill;
                break;
            case ct_wpn:
                RPDR->weapontype = itm_type;
                RPDR->weapongf = itm_gf;
                RPDR->weaponlevel = itm_level;
                RPDR->weaponskill = itm_skill;
                break;
            case ct_arm:
                RPDR->armortype = itm_type;
                RPDR->armorgf = itm_gf;
                RPDR->armorlevel = itm_level;
                RPDR->armorskill = itm_skill;
                break;
            case ct_pet:
                RPDR->pettype = itm_type;
                RPDR->petgf = itm_gf;
                RPDR->petlevel = itm_level;
                break;
            default:
                break;
            }
        }
        else if(id1 == avatar)
        {
            switch (Item_id.IdentifyItem(itm_type))
            {
            case ct_foot:
                RPDR->foot = itm_type;
                break;
            case ct_body:
                RPDR->body = itm_type;
                break;
            case ct_hand1:
                RPDR->hand1 = itm_type;
                break;
            case ct_hand2:
                RPDR->hand2 = itm_type;
                break;
            case ct_face:
                RPDR->face = itm_type;
                break;
            case ct_hair:
                RPDR->hair = itm_type;
                break;
            case ct_head:
                RPDR->head = itm_type;
                break;
            default:
                break;
            }
        }
    }
    mysql_free_result(res);
}

void MySQL::GetMoneyAmmount(int id,int *cash,unsigned __int64 *code,char sign,int cashcost,unsigned __int64 codecost)
{
    char buffer[300];
    if(sign)
    {
        if(cashcost)
            sprintf(buffer,"UPDATE users SET usr_cash = (usr_cash%c%d) WHERE usr_id = %d",sign,cashcost,id);

        if(codecost)
            sprintf(buffer,"UPDATE users SET usr_code = (usr_code%c%I64u) WHERE usr_id = %d",sign,codecost,id);

        if(cashcost || codecost)
            mysql_query(connection,buffer);
    }
    sprintf(buffer,"SELECT usr_cash, usr_code FROM users WHERE usr_id = %d",id);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    if(cash)*cash = atoi(result[0]);
    if(code)*code = _atoi64(result[1]);
    mysql_free_result(res);
}

void MySQL::UpgradeCard(MyCharInfo *Info,CardUpgradeResponse *CUR)
{
    char buffer[300];
    sprintf(buffer,"SELECT itm_type, itm_gf, itm_level FROM items WHERE itm_usr_id = %d AND itm_slot = %d",Info->usr_id,CUR->Slot);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    ItemId Item;
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    CUR->Type = atoi(result[0]);
    CUR->GF = atoi(result[1]);
    CUR->Level = atoi(result[2]);
    int EleCost = Item.GetUpgradeCost(CUR->Type,CUR->Level);
    int ItemSpirite = (CUR->Type%100)/10;
    if(ItemSpirite == 1)
    {
        Info->Water -= EleCost;
        sprintf(buffer,"UPDATE users SET usr_water = (usr_water-%d) WHERE usr_id = %d",EleCost,Info->usr_id);
    }
    else if(ItemSpirite == 2)
    {
        Info->Fire -= EleCost;
        sprintf(buffer,"UPDATE users SET usr_fire = (usr_fire-%d) WHERE usr_id = %d",EleCost,Info->usr_id);
    }
    else if(ItemSpirite == 3)
    {
        Info->Earth -= EleCost;
        sprintf(buffer,"UPDATE users SET usr_earth = (usr_earth-%d) WHERE usr_id = %d",EleCost,Info->usr_id);
    }
    else if(ItemSpirite == 4)
    {
        Info->Wind -= EleCost;
        sprintf(buffer,"UPDATE users SET usr_wind = (usr_wind-%d) WHERE usr_id = %d",EleCost,Info->usr_id);
    }
    CUR->WaterElements = Info->Water;
    CUR->FireElements = Info->Fire;
    CUR->EarthElements = Info->Earth;
    CUR->WindElements = Info->Wind;
    if(Item.UpgradeItem(CUR->GF,CUR->Level))
    {
        CUR->Level += 1;
    }
    else
    {
        CUR->UpgradeType = 2;
        CUR->unk2 = 5;
    }
    CUR->Skill = Item.GenerateSkill(CUR->Level,CUR->Type);
    mysql_free_result(res);
    mysql_query(connection,buffer);
    sprintf(buffer,"UPDATE items SET itm_level = %d, itm_skill = %d WHERE itm_usr_id = %d AND itm_slot = %d",CUR->Level,CUR->Skill,Info->usr_id,CUR->Slot);
    //cout << buffer << endl;
    mysql_query(connection,buffer);
}

void MySQL::GetScrolls(MyCharInfo *Info)
{
	char buffer[200];
    sprintf(buffer,"SELECT usr_scroll1,usr_scroll2,usr_scroll3 FROM users WHERE usr_id = %d",Info->usr_id);
	mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
	if(!result){
		printf("No Data\n");
		return;
	}
	for(int i = 0; i < 3; i++)
		Info->scrolls[i] = atoi(result[i]);
	mysql_free_result(res);
}

void MySQL::UpdateScrolls(int id,int slot,int scroll)
{
	char buffer[200];
    sprintf(buffer,"UPDATE users SET usr_scroll%d = %d WHERE usr_id = %d",slot+1,scroll,id);
    mysql_query(connection,buffer);
}

void MySQL::IncreaseMission(int id,int n)
{
	char buffer[200];
    sprintf(buffer,"UPDATE users SET usr_mission = usr_mission+%d WHERE usr_id = %d",n,id);
    mysql_query(connection,buffer);
}

void MySQL::SearchShop(CardSearchResponse *CSR,SearchType type)
{
	char buffer[200];
    sprintf(buffer,"SELECT * FROM cardshop WHERE shop_type = %d",type);
	mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
	CSR->total = res->row_count;
	MYSQL_ROW result = 0;
	int i = 0;
	while((result = mysql_fetch_row(res)) != 0 && i < 5){
	CSR->rooms[i] = atoi(result[2]);
	strcpy(CSR->name[i],result[3]);
	CSR->levels[i] = atoi(result[4]);
	CSR->gender[i] = atoi(result[5]);
	CSR->price[i] = _atoi64(result[6]);
	CSR->card[i] = atoi(result[7]);
	CSR->gf[i] = atoi(result[8]);
	CSR->level[i] = atoi(result[9]);
	CSR->skill[i] = atoi(result[10]);
	i++;
	}
	while(i < 5){
	CSR->rooms[i] = -1;
	CSR->name[i][0] = 0;
	CSR->levels[i] = -1;
	CSR->gender[i] = 0;
	CSR->price[i] = -1;
	CSR->card[i] = -1;
	CSR->gf[i] = -1;
	CSR->level[i] = -1;
	CSR->skill[i] = -1;
	i++;
	}
	mysql_free_result(res);
}

void MySQL::GetExp(int usr_id, int usr_exp,const char *Elements, int Ammount)
{
	char buffer[300];
	if(Elements)
	sprintf(buffer,"UPDATE users SET usr_points = (usr_points+%d),usr_code = (usr_code+%d),usr_%s = %d  WHERE usr_id = %d",usr_exp,usr_exp,Elements,Ammount,usr_id);
	else sprintf(buffer,"UPDATE users SET usr_points = (usr_points+%d),usr_code = (usr_code+%d)  WHERE usr_id = %d",usr_exp,usr_exp,usr_id);
	mysql_query(connection,buffer);
}