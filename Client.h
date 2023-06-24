#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#include "general.h"
class Client
{
public:
	char name[20];
	void KeyGen();

	Client(std::string name);
	void outsource(const uint8_t* s_key, const uint8_t* cipher_key);
	void encode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* s_key);
	bool Verify(uint8_t* digestA, uint8_t* digestB);
};

#endif
