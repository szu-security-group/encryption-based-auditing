#pragma once
#ifndef SERVER
#define SERVER
#include<string>
#include "Client.h"
#include "general.h"
#include "gfmul.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <crypto870/filters.h>
#include <crypto870/hex.h>

class Server
{
public:
	char name[20];
	Server(std::string name);
	void response(chall t_chall, const std::string file_name, uint8_t* proof);
};

#endif // !SERVER

