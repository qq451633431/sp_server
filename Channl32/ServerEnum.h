#ifndef __SERVERENUM_H__
#define __SERVERENUM_H__

#define MISSION_MODE	0x2E

enum Character
{
    None = 0,
    Xyrho = 10,
    Shamoo = 20,
    Seven = 30,
    Cream = 40,
    Roland = 50,
    Aurelli = 60,
    Hawk = 70,
    Hazel = 80,
    Cara = 90,
    Warren = 100,
    Lucy = 110,
    Will = 120,
    PremXyrho = 130,
    PremShamoo = 140,
    PremSeven = 150,
    PremCream = 160,
    PremRoland = 170,
    PremAurelli = 180,
    PremHawk = 190,
    PremHazel = 200,
    PremCara = 210,
    PremWarren = 220,
    PremLucy = 230,
    PremWill = 240,

};

enum ResponseType
{
    WrongPasswd = 0,
    CorrectPasswd = 1
};

enum ItemType
{
    ct_pet = 0,
    ct_wpn = 1,
    ct_arm = 2,
    ct_mag = 3
};

enum AvatarType
{
    ct_foot = 1,
    ct_body = 2,
    ct_hand1 = 3,
    ct_hand2 = 4,
    ct_face = 5,
    ct_hair = 6,
    ct_head = 7
};

enum SearchType
{
	st_Spirit = 0,
	st_Axe = 11,
	st_Staff = 12,
	st_Sword = 13,
	st_Bow = 14,
	st_Shield = 21,
	st_Pendant = 22,
	st_Armour = 23,
	st_Boots = 24,
	st_Magic = 31,
	st_Cash = 90,
	st_GoldForce = 91,
	st_Avatar = 92
};

enum NpcType {
	NPC_TYPE_GOLEM = 1,
	NPC_TYPE_SOLDIER = 2,
	NPC_TYPE_ARCHER = 3,
	NPC_TYPE___UNKNOWN1 = 4, // I tested all modes and didn't see any NPC with this type
	NPC_TYPE_NIGHT_SOLDIER = 5,
	NPC_TYPE_DEATH = 6,
	NPC_TYPE_SLIME = 7,
	NPC_TYPE_GARGOYLE = 8,
	NPC_TYPE_TREASURE_BOX = 9,
	NPC_TYPE_MASTER_SOLDIER = 10,
	NPC_TYPE_BARLOG = 11,
	NPC_TYPE_GOBLIN = 12,
	NPC_TYPE_GOBLIN_SHAMAN = 13,
	NPC_TYPE_HORNED_OWL = 14,
	NPC_TYPE_DARK_TREE = 15,
	NPC_TYPE___UNKNOWN2 = 16, // I tested all modes and didn't see any NPC with this type
	NPC_TYPE_ORC = 17,
	NPC_TYPE_PHANTOM = 18,
	NPC_TYPE_ORC_BERSERK = 19,
	NPC_TYPE_ORC_MAGE = 20,
	NPC_TYPE_CYCLOPS = 21,
	NPC_TYPE_DARK_FAIRY = 22,
	NPC_TYPE_MAGMA_GOLEM = 23,
	NPC_TYPE_HELL_HOUND = 24,
	NPC_TYPE_MAGMA_BOMBER = 25,
	NPC_TYPE_DARK_ORB = 26,
	NPC_TYPE_SEAL_STONE = 27,
	NPC_TYPE_CERBERUS = 28,
	NPC_TYPE_BURNING_SOUL = 29,
	NPC_TYPE_EFREET = 30,
	NPC_TYPE_RED_DRAGON = 31,
	NPC_TYPE_BARRICADE = 32,
	NPC_TYPE_DODGE_SNOWMAN = 33,
	NPC_TYPE_MOLE = 34,
	NPC_TYPE_SYMBOL = 35,
	NPC_TYPE_REAL_CHARACTER = 36,
	NPC_TYPE_HOCKEY_GOALIE = 37,
	NPC_TYPE_HOCKEY_NET = 38,
	NPC_TYPE_PET_SLIME = 39,
	NPC_TYPE_PET_SKULL_MAGE = 40,
	NPC_TYPE_PET_GOBLIN_SHAMAN = 41,
	NPC_TYPE_PET_GOLEM = 42,
	NPC_TYPE_PET_MAGMA_GOLEM = 43,
	NPC_TYPE_PET_SANTA_CLAUS = 44,
	NPC_TYPE_PET_BEAGLE = 45,
	NPC_TYPE_PET_LEO = 47,
	NPC_TYPE_PET_SOCCER_LEO = 48,
	NPC_TYPE_PET_RED_DRAGON = 49,
	NPC_TYPE_PET_MINI_PIG = 50,
	NPC_TYPE_PET_MAGIC_PENGUIN = 51,
	NPC_TYPE_PET_SNOWMAN = 52,
	NPC_TYPE_TRAINING_TARGET = 100,
	NPC_TYPE_TRAINING_BARREL = 101,
	NPC_TYPE_QUEST_CRYSTAL = 102
};

struct MyCharInfo
{
    int usr_id,usr_room,usr_ready,usr_slot,usr_char,rm_master,usr_mode, usr_team;
	char usr_name[13];
    int UserType, premium;
    bool gender, admin, questlife;
    int DefaultCharacter;
    int Level;
    unsigned __int64 Points;
    unsigned __int64 Code;
    int Coins;
    int Mission;
    int nSlots;
    int Water, Fire, Earth, Wind;
	int scrolls[3];
};

struct Equips
{
    int magic;
    int weapon;
    int armour;
    int pet;
    //Avatar
};

enum CardType1
{
    black = 1,
    silver = 2,
    gold = 3,
    avatar = 4,
    pet = 5,
    silverp = 7
};

enum CardType2
{
    none = 0,
    water = 1*10,
    fire = 2*10,
    earth = 3*10,
    wind = 4*10
};


enum Weapon
{
    axe = 1+100+1000,
    staff = 2+100+1000,
    sword = 3+100+1000,
    bow = 4+100+1000
};

enum Armor
{
    shield = 1+200+1000,
    pendant = 2+200+1000,
    armor = 3+200+1000,
    boots = 4+200+1000
};

enum Magic
{
    normalmagic = 1+300+1000,
    specialmagic = 2+300+1000
};


class Card
{
public:
    Card(Weapon a, CardType2 b)
    {
        Type = a + b;
    }
    Card(Armor a, CardType2 b)
    {
        Type = a + b;
    }
    Card(Magic a, CardType2 b)
    {
        Type = a + b;
    }
    int Type;
    int getType()
    {
        return Type;
    };
};

enum Skill1
{


};

class Skill
{
public:
    Skill(int a,int b, int c)
    {
        skill = a*1000000 + b*1000 + c;
    }
    int skill;
    int getType()
    {
        return skill;
    };
};

const int MagicEleLvlCost[] = {4,7,13,50,100,800,1600,3200};
const int WpnEleLvlCost [] = {2,4,7,25,50,400,800,1600};
const int ArmEleLvlCost [] = {1,2,4,13,25,200,400,800};
const int ShdEleLvlCost [] = {1,1,2,7,13,100,200,400};


const int BootSkill1[] = {110,111,112,113,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171};
const int ArmorSkill1[] = {102,103,104,105,106,107,108,109,142,143,144,145,146,147,148,149,150,151,152,153,154,155};
const int WeaponSkill1[] = {100,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141};
const int BootSkill2[] = {310,311,312,313,349,350,351,352,361,362,363,364,365,366,367,368,369,370,371,372};
const int MeleeWpnSkill2[] = {200,201,300,301,302,303,304,306,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435};
const int RangedWpnSkill2[] = {203,204,300,301,302,303,304,306,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435};
const int MagicSkill[] = {300,301,302,303,304,306,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435};
static const char * ChatTypes[] = {"All","Whisper","Friend","Guild","GuildUnk","Unk","GM","Trade"};
static const char * ElementTypes[] = {"none","water","fire","earth","wind"};
class ItemId
{
private:
public:
    int Identify(int a)
    {
        return a/1000;
    }
    int IdentifyItem(int a)
    {
        return (a/100)%10;
    }
    int IdentifyType(int a)
    {
        return a%10;
    }
    bool UpgradeItem(int gf,int level)
    {
        if(level < 5)return true;
        int x = rand() % 100;
        int range = 0;
        if(level == 5)
            range = 20;
        else if(level == 6)
            range = 10;
        else if(level == 7)
            range = 5;
        if(gf)range *= 2;
        if(x < range)return true;
        return false;
    }
    int GetUpgradeCost(int type,int level)
    {
        switch(IdentifyItem(type))
        {
            case ct_mag:
            return MagicEleLvlCost[level];
            case ct_wpn:
            return WpnEleLvlCost[level];
            case ct_arm:
            if(type%10 == 1)return ShdEleLvlCost[level];
            return ArmEleLvlCost[level];
            default:
            break;
        }
        return 0;
    }
    int GenerateSkill(int level,int Type)
    {
        int a = Type%10, b = IdentifyItem(Type);
        int Skill1 = rand()%((level+1)*4) + 1, Skill2 = 0, Skill3 = 0;
        if(a == 4 && b == ct_arm ) //boots
        {
            Skill2 = BootSkill1[rand()%20];
            Skill3 = BootSkill2[rand()%20];
            Skill Sk(Skill1,Skill2,Skill3);
            return Sk.getType();
        }
        if(a == 3 && b == ct_arm) //armor
        {
            Skill2 = ArmorSkill1[rand()%22];
            Skill Sk(Skill1,Skill2,Skill3);
            return Sk.getType();
        }
        if(b == ct_wpn)
        {
            Skill2 = WeaponSkill1[rand()%25];
            if(a == 1 || a == 3)
            {
                Skill3 = MeleeWpnSkill2[rand()%106];
            }
            else if(a == 2 || a == 4)
            {
                Skill3 = RangedWpnSkill2[rand()%106];
            }
            Skill Sk(Skill1,Skill2,Skill3);
            return Sk.getType();
        }
        if(b == ct_mag)
        {
            Skill2 = MagicSkill[rand()%104];
            Skill3 = MagicSkill[rand()%104];
            Skill Sk(Skill1,Skill2,Skill3);
            return Sk.getType();
        }
    }
};

struct ExpGain
{
	int cpMul[8];
	int exp[8];
};
class ExpCalculator
{
private:
	int ch_multiplier;
public:
	ExpCalculator(int multiplier)
	{
		ch_multiplier = multiplier;
	}
	int CalculateNpcExp(int npcType)
	{
		return 7000;
	}
	void Calculate(short npcType, ExpGain *gain,int *damages)
	{
		if(damages)
		{
			int totaldmg = 0, totalexp = CalculateNpcExp(npcType);
			for(int i = 0; i < 8; i++)
				totaldmg += damages[i];
			for(int i = 0; i < 8; i++)
				if(damages[i]){
					gain->exp[i] = totalexp*(1.0*damages[i]/totaldmg);
					gain->cpMul[i] = rand()%10+1;
				}else gain->exp[i] = 0;
		}
	}

};

#endif
