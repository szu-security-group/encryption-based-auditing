#include "Client.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <crypto870/osrng.h>
#include <crypto870/salsa.h>
#include "gfmul.h"
#include "timer.h"
#include "general.h"



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
	std::ifstream infile(key_path);
	if (!infile.good()) {
		const unsigned int keySize = CryptoPP::AES::DEFAULT_KEYLENGTH;
		CryptoPP::AutoSeededRandomPool rng;
		rng.GenerateBlock(key_A, keySize);
		rng.GenerateBlock(key_B, keySize);
		rng.GenerateBlock(s_a, keySize);
		rng.GenerateBlock(s_b, keySize);
		rng.GenerateBlock(key_digest_a, keySize);
		rng.GenerateBlock(key_digest_b, keySize);
		std::ofstream outfile(key_path);
		save_key();
	}
	else {
		read_key();
	}
}


Client::Client(std::string name)
{
	name.copy(this->name, name.length());
}

void Client::test_Decode_and_Encode(const uint8_t* cipher_key, const uint8_t* s_key)
{
	std::ifstream infile(file_path, std::ios::in | std::ios::binary);
	try {
		std::stringstream  file_str_stream;
		file_str_stream << infile.rdbuf();
		infile.close();
		unsigned long long _file_size = std::filesystem::file_size(file_path);
		int padding_length = 0;
		if (_file_size % AES128_BLOCK_SIZE != 0) {
			padding_length = AES128_BLOCK_SIZE - _file_size % AES128_BLOCK_SIZE;
			file_str_stream << std::setfill((char)0) << std::setw(padding_length) << (char)0;
		}
		std::string file_str = file_str_stream.str();
		unsigned long long t_length = file_str.length();
		uint8_t* CIPHERTEXT = (uint8_t*)malloc(t_length);
		encode((uint8_t*)file_str.c_str(), CIPHERTEXT, t_length, cipher_key,s_key);
		
		save_file(CIPHERTEXT, t_length, file_a);
		uint8_t* DECIPHERTEXT = (uint8_t*)malloc(t_length);
		__m128i e_s;
		__m128i s = _mm_loadu_si128((__m128i*)s_key);
		getInverseEle(s, e_s);
		decode(CIPHERTEXT, DECIPHERTEXT, t_length,cipher_key, (uint8_t*)&e_s);
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
}

chall Client::challenge()
{
	chall t_chall;
	const unsigned int keySize = CryptoPP::Salsa20::DEFAULT_KEYLENGTH;
	CryptoPP::AutoSeededRandomPool rng;
	rng.GenerateBlock(t_chall.index, keySize);
	rng.GenerateBlock(t_chall.coeff, keySize);
	return t_chall;
}



void Client::encode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* m_key, const uint8_t* s_key)
{
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i* _128_in = (__m128i*)in;
	__m128i* _128_out = (__m128i*)out;
	__m128i s = *(__m128i*)s_key;
	s = _mm_shuffle_epi8(s, bswap_mask);

	__m128i ctr_block = _mm_set_epi64x(0, 0), tmp1, tmp2, ONE, BSWAP_EPI64;
	int i, j;
	ONE = _mm_set_epi32(0, 1, 0, 0);
	BSWAP_EPI64 = _mm_setr_epi8(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8);
	ctr_block = _mm_insert_epi64(ctr_block, *(long long*)CTR128_IV, 1);
	ctr_block = _mm_insert_epi32(ctr_block, *(long*)CTR128_NONCE, 1);
	ctr_block = _mm_srli_si128(ctr_block, 4);
	ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
	ctr_block = _mm_add_epi64(ctr_block, ONE);
	AES_KEY aes_key;
	AES_set_encrypt_key(m_key, 128, &aes_key);

	for (int i = 0; i < length / AES128_BLOCK_SIZE; i++) {
		// product coefficient
		tmp1 = _mm_shuffle_epi8(*(_128_in + i), bswap_mask);
		gfmul_(tmp1, s, tmp1);
		// aes ctr crypt
		tmp2 = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
		ctr_block = _mm_add_epi64(ctr_block, ONE);
		tmp2 = _mm_xor_si128(tmp2, ((__m128i*)aes_key.KEY)[0]);
		for (j = 1; j < aes_key.nr; j++) {
			tmp2 = _mm_aesenc_si128(tmp2, ((__m128i*)aes_key.KEY)[j]);
		};
		tmp2 = _mm_aesenclast_si128(tmp2, ((__m128i*)aes_key.KEY)[j]);
		tmp2 = _mm_xor_si128(tmp2, tmp1);
		_mm_storeu_si128(&((__m128i*)out)[i], tmp2);
	}

}


void Client::decode(const uint8_t* in, uint8_t* out, unsigned long long length, const uint8_t* m_key, const uint8_t* s_key)
{
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i* _128_in = (__m128i*)in;
	__m128i* _128_out = (__m128i*)out;
	__m128i s = *(__m128i*)s_key;
	s = _mm_shuffle_epi8(s, bswap_mask);

	__m128i ctr_block = _mm_set_epi64x(0, 0), tmp1, tmp2, ONE, BSWAP_EPI64;
	int i, j;
	ONE = _mm_set_epi32(0, 1, 0, 0);
	BSWAP_EPI64 = _mm_setr_epi8(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8);
	ctr_block = _mm_insert_epi64(ctr_block, *(long long*)CTR128_IV, 1);
	ctr_block = _mm_insert_epi32(ctr_block, *(long*)CTR128_NONCE, 1);
	ctr_block = _mm_srli_si128(ctr_block, 4);
	ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
	ctr_block = _mm_add_epi64(ctr_block, ONE);
	AES_KEY aes_key;
	AES_set_encrypt_key(m_key, 128, &aes_key);

	for (int i = 0; i < length / AES128_BLOCK_SIZE; i++) {
		tmp2 = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
		ctr_block = _mm_add_epi64(ctr_block, ONE);
		tmp2 = _mm_xor_si128(tmp2, ((__m128i*)aes_key.KEY)[0]);
		for (j = 1; j < aes_key.nr; j++) {
			tmp2 = _mm_aesenc_si128(tmp2, ((__m128i*)aes_key.KEY)[j]);
		};
		tmp2 = _mm_aesenclast_si128(tmp2, ((__m128i*)aes_key.KEY)[j]);
		tmp2 = _mm_xor_si128(tmp2, *(_128_in + i));
		gfmul_(tmp2, s, tmp1);
		tmp1 = _mm_shuffle_epi8(tmp1, bswap_mask);
		_mm_storeu_si128(&((__m128i*)out)[i], tmp1);
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

