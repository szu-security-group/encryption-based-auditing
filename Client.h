#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "general.h"
#include <crypto870/salsa.h>

typedef struct chall {
	uint8_t index[CryptoPP::Salsa20::DEFAULT_KEYLENGTH];
	uint8_t coeff[CryptoPP::Salsa20::DEFAULT_KEYLENGTH];
}chall;

class Client
{
public:
	char name[20];
	void keyGen();

	Client(std::string name);

	void encode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* m_key, const uint8_t* s_key);
	void decode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* m_key, const uint8_t* s_key);
	void test_Decode_and_Encode();
	chall challenge();
	bool Verify(chall t_chall,uint8_t* digestA, uint8_t* digestB);
};

#endif
