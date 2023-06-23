#include "general.h"
#include "gfmul.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

#include <crypto870/cryptlib.h>
#include <crypto870/salsa.h>
#include <crypto870/files.h>
#include <wmmintrin.h> 
#include <emmintrin.h> 
#include <smmintrin.h> 

void print_buffer(const uint8_t* pointer)
{

    for (int i = 0; i < BUFFER_SIZE; i++)
        printf("%02x ", pointer[i]);
    printf("\n");
}

void print_box(const uint8_t* box)
{
    for (int i = 0; i < 256; i++) {
        printf("%02x ", box[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
}

void print_bytes(const uint8_t* pointer, int length)
{
    for (int i = 0; i < length; i++) {
        printf("0X%02x, ", pointer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
}

block multiply_sum(block* a, block* b, int length) {
    block sum = 0;
    for (int i = 0; i < length; i = i + 1) {
        sum = sum + a[i] * b[i];
    }
    return sum;
}

void test_salsa20_stream()
{
    using namespace CryptoPP;
    Salsa20::Encryption enc;
    uint8_t key[16] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t iv[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    enc.SetKeyWithIV(key, 16, iv, 8);
    uint8_t plaintext[64] = {};
    uint8_t ciphertext[64] = {};
    enc.ProcessData(ciphertext, plaintext, 64);
    uint8_t plaintext1[64] = {};
    uint8_t ciphertext1[64] = {};
    enc.ProcessData(ciphertext1, plaintext1, 64);
    print_bytes(ciphertext, 64);
    print_bytes(ciphertext1, 64);
}


std::string binaryToHex(std::string binaryStr)
{
    std::string ret;
    static const char* hex = "0123456789ABCDEF";
    for (auto c : binaryStr)
    {
        ret.push_back(hex[(c >> 4) & 0xf]); //取二进制高四位
        ret.push_back(hex[c & 0xf]);        //取二进制低四位
    }
    return ret;
}

/// <summary>
/// 
/// </summary>
/// <param name="in"></param>
/// <param name="out"></param>
/// <param name="ivec"></param>
/// <param name="nonce"></param>
/// <param name="length"></param>
/// <param name="key"></param>
/// <param name="number_of_rounds"></param>
void AES_CTR_encrypt(const uint8_t* in,unsigned char* out,const uint8_t ivec[8],
    const uint8_t nonce[4],unsigned long length,const uint8_t* key,int number_of_rounds){
    __m128i ctr_block, tmp, ONE, BSWAP_EPI64;
    int i, j;
    if (length % 16)
        length = length / 16 + 1;
    else length /= 16;
    ONE = _mm_set_epi32(0, 1, 0, 0);
    BSWAP_EPI64 = _mm_setr_epi8(7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8);
    ctr_block = _mm_insert_epi64(ctr_block, *(long long*)ivec, 1);
    ctr_block = _mm_insert_epi32(ctr_block, *(long*)nonce, 1);
    ctr_block = _mm_srli_si128(ctr_block, 4);
    ctr_block = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
    ctr_block = _mm_add_epi64(ctr_block, ONE);
    for (i = 0; i < length; i++) {
        tmp = _mm_shuffle_epi8(ctr_block, BSWAP_EPI64);
        ctr_block = _mm_add_epi64(ctr_block, ONE);
        tmp = _mm_xor_si128(tmp, ((__m128i*)key)[0]);
        for (j = 1; j < number_of_rounds; j++) {
            tmp = _mm_aesenc_si128(tmp, ((__m128i*)key)[j]);
        };
        tmp = _mm_aesenclast_si128(tmp, ((__m128i*)key)[j]);
        tmp = _mm_xor_si128(tmp, _mm_loadu_si128(&((__m128i*)in)[i]));
        _mm_storeu_si128(&((__m128i*)out)[i], tmp);
    }
}


