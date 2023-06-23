#include "Client.h"
#include "general.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <crypto870/osrng.h>
#include "timer.h"


/// <summary>
/// 
/// </summary>
/// <param name="key_A">key for ServerA</param>
/// <param name="key_B">key for ServerB</param>
/// <param name="s_a">secrect a</param>
/// <param name="s_b">secrect b</param>
/// <param name="k_d_a">key to generate digest for ServerA</param>
/// <param name="k_d_b">key to generate digest for ServerB</param>
void Client::KeyGen(uint8_t* key_A, uint8_t* key_B, uint8_t* s_a, uint8_t* s_b, uint8_t* k_d_a, uint8_t* k_d_b)
{
	const unsigned int keySize = CryptoPP::AES::DEFAULT_KEYLENGTH;
	CryptoPP::AutoSeededRandomPool rng;
	rng.GenerateBlock(key_A, keySize);
	rng.GenerateBlock(key_B, keySize);
	rng.GenerateBlock(s_a, keySize);
	rng.GenerateBlock(s_b, keySize);
	rng.GenerateBlock(k_d_a, keySize);
	rng.GenerateBlock(k_d_b, keySize);
}


Client::Client(std::string name)
{
	name.copy(this->name, name.length());
}

bool Client::Verify(uint8_t* digestA, uint8_t* digestB)
{
	timer time1 = timer();
	time1.set_start();
	uint8_t tmp[BLOCK_SIZE]{};
	uint64_t* p = (uint64_t*)tmp;
	for (int i = 0; i < BLOCK_SIZE; i++)
		tmp[i] = digestA[i] ^ digestB[i];
	time1.set_end();
	time1.compute_duration();
	time1.print_time_cost();
	if (*p == 0)
		return true;
	else
		return false;
}



