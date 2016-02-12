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

int MySQL::Login(char* id, char* pw)
{
    char buffer[200];
    sprintf(buffer,"SELECT usr_id FROM users WHERE usr_name = \'%s\' AND usr_pw = \'%s\'",id,pw);
    mysql_query(connection,buffer);
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)return 0;
    int x = atoi(result[0]);
    mysql_free_result(res);
    return x;
}

void MySQL::GetUserInfo(int id, MyCharInfo &info)
{
    char buffer[200];
    sprintf(buffer,"SELECT usr_char, usr_points, usr_code, usr_level FROM users WHERE usr_id = %d",id);
    if(mysql_query(connection,buffer))
        printf(mysql_error(connection));
    MYSQL_RES *res = mysql_use_result(connection);
    MYSQL_ROW result = mysql_fetch_row(res);
    if(!result)
    {
        printf("No Data\n");
        return;
    }
    info.DefaultCharacter = atoi(result[0]);
    info.Points = _atoi64(result[1]);
    info.Code = _atoi64(result[2]);
    info.Level = _atoi64(result[3]);
	if(info.Level > 0 && info.Level <= 12)
		info.UserType = 10;
	else if(info.Level > 12 && info.Level <= 16)
		info.UserType = 20;
	else if(info.Level > 16)
		info.UserType = 30;
	else info.UserType = 0;
    mysql_free_result(res);
	Level level;
	int l = level.getLevel(info.Points);
	if(info.Level != l){
			sprintf(buffer,"UPDATE users SET usr_level = %d WHERE usr_id = %d",l,id);
		if(mysql_query(connection,buffer))
			printf(mysql_error(connection));
		info.Level = l;
	}
}

void MySQL::SetDefaultCharacter(int id,Character DefaultCharacter)
{
    char buffer[200];
    sprintf(buffer,"UPDATE users SET usr_char = %d WHERE usr_id = %d",DefaultCharacter,id);
    mysql_query(connection,buffer);
}
