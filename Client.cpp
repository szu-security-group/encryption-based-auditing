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

//size_t maxIndex = std::filesystem::file_size(file_a) % AES128_BLOCK_SIZE;

void Client::keyGen()
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

void Client::test_Decode_and_Encode()
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
		maxIndex = t_length / AES128_BLOCK_SIZE;
		uint8_t* CIPHERTEXT = (uint8_t*)malloc(t_length);
		encode((uint8_t*)file_str.c_str(), CIPHERTEXT, t_length, key_A,s_a);
		save_file(CIPHERTEXT, t_length, file_a);
		encode((uint8_t*)file_str.c_str(), CIPHERTEXT, t_length, key_B, s_b);
		save_file(CIPHERTEXT, t_length, file_b);
		uint8_t* DECIPHERTEXT = (uint8_t*)malloc(t_length);
		__m128i e_s;
		__m128i s = _mm_loadu_si128((__m128i*)s_a);
		getInverseEle(s, e_s);
		decode(CIPHERTEXT, DECIPHERTEXT, t_length,key_A, (uint8_t*)&e_s);
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


bool Client::Verify(chall t_chall, uint8_t* digestA, uint8_t* digestB)
{

	__m128i gamma_a = *(__m128i*)digestA;
	__m128i gamma_b = *(__m128i*)digestB;

	std::vector<uint32_t> indices;
	indices = getRandomIndex(t_chall.index,maxIndex);
	std::string str_coeff;
	char buffer[AES128_BLOCK_SIZE];
	str_coeff = getCoeff(t_chall.coeff);
	__m128i* __128coeff = (__m128i*)str_coeff.c_str();
	__m128i sum_a = _mm_set_epi32(0, 0, 0, 0);
	__m128i sum_b = _mm_set_epi32(0, 0, 0, 0);

	__m128i ctr_block = _mm_set_epi64x(0, 0), tmp1, tmp2, tmp3, tmp4, BSWAP_EPI64;
	int i, j;
	BSWAP_EPI64 = _mm_setr_epi8(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8);
	ctr_block = _mm_insert_epi64(ctr_block, *(long long*)CTR128_IV, 1);
	ctr_block = _mm_insert_epi32(ctr_block, *(long*)CTR128_NONCE, 1);
	ctr_block = _mm_srli_si128(ctr_block, 4);
	ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);

	AES_KEY aes_key_a;
	AES_set_encrypt_key(key_A, 128, &aes_key_a);
	AES_KEY aes_key_b;
	AES_set_encrypt_key(key_B, 128, &aes_key_b);

	for (i = 0; i < CHALLENGE_NUM; i++) {
		ctr_block = _mm_insert_epi32(ctr_block, indices[i]+1, 2);
		tmp1 = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
		tmp1 = _mm_xor_si128(tmp1, ((__m128i*)aes_key_a.KEY)[0]);
		for (j = 1; j < aes_key_a.nr; j++) {
			tmp1 = _mm_aesenc_si128(tmp1, ((__m128i*)aes_key_a.KEY)[j]);
		}
		tmp1 = _mm_aesenclast_si128(tmp1, ((__m128i*)aes_key_a.KEY)[j]);
		gfmul_(tmp1, *__128coeff, tmp2);
		sum_a = _mm_xor_si128(sum_a, tmp2);

		tmp3 = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
		tmp3 = _mm_xor_si128(tmp3, ((__m128i*)aes_key_b.KEY)[0]);
		for (j = 1; j < aes_key_b.nr; j++) {
			tmp3 = _mm_aesenc_si128(tmp3, ((__m128i*)aes_key_b.KEY)[j]);
		}
		tmp3 = _mm_aesenclast_si128(tmp3, ((__m128i*)aes_key_b.KEY)[j]);
		gfmul_(tmp3, *__128coeff, tmp4);
		sum_b = _mm_xor_si128(sum_b, tmp4);
		++__128coeff;
	}
	gamma_a = _mm_xor_si128(sum_a, gamma_a);
	gamma_b = _mm_xor_si128(sum_b, gamma_b);
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i a = *(__m128i*)s_a;
	__m128i b = *(__m128i*)s_b;
	a = _mm_shuffle_epi8(a, bswap_mask);
	b = _mm_shuffle_epi8(b, bswap_mask);
	gfmul_(gamma_a, b, tmp1);
	gfmul_(gamma_b, a, tmp3);
	int mask = _mm_movemask_epi8(_mm_cmpeq_epi32(tmp1, tmp3));
	return mask == 0xFFFF;
}

