#pragma once
#ifndef GENERAL_H
#define GENERAL_H
#include <iostream>
#include <vector>
#include <string>


constexpr auto BLOCK_SIZE = 8;
constexpr auto AES128_BLOCK_SIZE = 16;
constexpr auto BUFFER_SIZE = 2048;
constexpr auto KEY_SIZE = 16;
// challenge length 460(688) leads to 99% (99.9%) a successful detection probability 
// if 1% outsourced data is damaged.
constexpr auto CHALLENGE_NUM = 460;
// constexpr auto CHALLENGE_NUM = 688;

constexpr auto file_path = "E:\\home\\data\\1.txt";
constexpr auto file_a = "E:\\home\\data\\encrypt_a.enc";
constexpr auto file_b = "E:\\home\\data\\encrypt_b.enc";
typedef unsigned long long block;

extern uint8_t key_A[KEY_SIZE];
extern uint8_t key_B[KEY_SIZE];
extern uint8_t s_a[KEY_SIZE];
extern uint8_t s_b[KEY_SIZE];
extern uint8_t key_digest_a[KEY_SIZE];
extern uint8_t key_digest_b[KEY_SIZE];
extern uint8_t CTR128_IV[8];
extern uint8_t CTR128_NONCE[4];

typedef struct KEY_SCHEDULE {
    uint8_t KEY[16 * 15];
    unsigned int nr;
}AES_KEY;

void test_salsa20_stream();

void print_buffer(const uint8_t* pointer);

void print_box(const uint8_t* box);

void print_bytes(const uint8_t* pointer, int length);

std::string binaryToHex(std::string binaryStr);


int AES_set_encrypt_key(const uint8_t* userKey,const int bits,AES_KEY* key);

void AES_CTR_encrypt(const uint8_t* in,
    unsigned char* out,
    const uint8_t ivec[8],
    const uint8_t nonce[4],
    unsigned long length,
    const uint8_t* key,
    int number_of_rounds);

void test_AES_CTR();

std::string getCoeff(const uint8_t* key);

std::vector<uint32_t> getRandomIndex(const uint8_t* key, uint32_t file_length);

void save_file(const uint8_t* data,std::size_t size,std::string file_name);

#endif
