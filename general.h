#pragma once
#include <iostream>
#include <string>

#ifndef GENERAL_H
#define GENERAL_H

constexpr auto BLOCK_SIZE = 8;
constexpr auto AES_BLOCK_SIZE = 16;
constexpr auto BUFFER_SIZE = 2048;
constexpr auto KEY_SIZE = 16;
// challenge length 460(688) leads to 99% (99.9%) a successful detection probability 
// if 1% outsourced data is damaged.
constexpr auto CHALLENGE_NUM = 460;
// constexpr auto CHALLENGE_NUM = 688;

constexpr auto file_path = "E:\\home\\data\\3680.txt";
typedef unsigned long long block;

extern uint8_t key_A[KEY_SIZE];
extern uint8_t key_B[KEY_SIZE];
extern uint8_t s_a[KEY_SIZE];
extern uint8_t s_b[KEY_SIZE];
extern uint8_t k_d_a[KEY_SIZE];
extern uint8_t k_d_b[KEY_SIZE];


void test_salsa20_stream();

void print_buffer(const uint8_t* pointer);

void print_box(const uint8_t* box);

void test_AES_CTR();

void print_bytes(const uint8_t* pointer, int length);



void AES_CTR_encrypt(const uint8_t* in,
    unsigned char* out,
    const uint8_t ivec[8],
    const uint8_t nonce[4],
    unsigned long length,
    const uint8_t* key,
    int number_of_rounds);

std::string binaryToHex(std::string binaryStr);

#endif
