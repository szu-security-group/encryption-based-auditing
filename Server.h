#pragma once
#include <string>
#include "Client.h"
#include "general.h"
#include "gfmul.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <crypto++/filters.h>
#include <crypto++/hex.h>

class Server
{
public:
  char name[20];
  Server(std::string name);
  void response(chall t_chall, const std::string file_name, uint8_t *proof);
};
