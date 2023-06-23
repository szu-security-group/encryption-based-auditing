#pragma once
#include "general.h"
class Client
{
public:
	char name[20];
	void KeyGen(uint8_t* key_A, uint8_t* key_B, uint8_t* s_a, uint8_t* s_b, uint8_t* k_d_a, uint8_t* k_d_b);

	Client(std::string name);

	bool Verify(uint8_t* digestA, uint8_t* digestB);
};

