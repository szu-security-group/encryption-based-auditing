#include "Client.h"
#include "general.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <crypto870/osrng.h>
#include "gfmul.h"
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
void Client::KeyGen()
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

void Client::outsource(const uint8_t* s_key,const uint8_t* cipher_key)
{
	std::ifstream infile(file_path, std::ios::in | std::ios::binary);
	try {
		std::stringstream  file_str_stream;
		file_str_stream << infile.rdbuf();
		infile.close();
		unsigned long long _file_size = std::filesystem::file_size(file_path);
		int padding_length = sizeof(block) - _file_size % sizeof(block);
		file_str_stream << std::setfill((char)0) << std::setw(padding_length) << (char)0;
		std::string file_str = file_str_stream.str();
		unsigned long long t_length = file_str.length();
		uint8_t* CIPHERTEXT = (uint8_t*)malloc(t_length);
		encode((uint8_t*)&file_str, CIPHERTEXT, t_length, s_key);
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
}



void Client::encode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* s_key)
{
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i* _128_in = (__m128i*)in;
	__m128i* _128_out = (__m128i*)out;
	__m128i s = *(__m128i*)s_key;
	for (int i = 0; i < length / AES_BLOCK_SIZE; i++) {
		__m128i tmp = _mm_shuffle_epi8(*(_128_in + i), bswap_mask);
		gfmul_(tmp, s, *(_128_out + i));
	}
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



