#ifndef __MYSQL_H__
#define __MYSQL_H__

#include "main.h"
#include "ServerEnum.h"
#include <mysql.h>

class MySQL
{
private:
MYSQL *connection;

public:
MySQL();
~MySQL();
int Login(char*,char*);
void GetUserInfo(int,MyCharInfo&);
void SetDefaultCharacter(int,Character);

};


#endif
