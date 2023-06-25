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

uint8_t key_A[KEY_SIZE]{};
uint8_t key_B[KEY_SIZE]{};
uint8_t s_a[KEY_SIZE]{};
uint8_t s_b[KEY_SIZE]{};
uint8_t key_digest_a[KEY_SIZE]{};
uint8_t key_digest_b[KEY_SIZE]{};
uint8_t CTR128_IV[8] = { 0xC0,0x54,0x3B,0x59,0xDA,0x48,0xD9,0x0B };
uint8_t CTR128_NONCE[4] = { 0x00,0x6C,0xB6,0xDB };



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


inline __m128i AES_128_ASSIST(__m128i temp1, __m128i temp2)
{
    __m128i temp3;
    temp2 = _mm_shuffle_epi32(temp2, 0xff);
    temp3 = _mm_slli_si128(temp1, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp3 = _mm_slli_si128(temp3, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp3 = _mm_slli_si128(temp3, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp1 = _mm_xor_si128(temp1, temp2);
    return temp1;
}

void AES_128_Key_Expansion(const uint8_t* userkey,
    uint8_t* key)
{
    __m128i temp1, temp2;
    __m128i* Key_Schedule = (__m128i*)key;

    temp1 = _mm_loadu_si128((__m128i*)userkey);
    Key_Schedule[0] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x1);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[1] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x2);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[2] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x4);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[3] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x8);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[4] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x10);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[5] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x20);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[6] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x40);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[7] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x80);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[8] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x1b);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[9] = temp1;
    temp2 = _mm_aeskeygenassist_si128(temp1, 0x36);
    temp1 = AES_128_ASSIST(temp1, temp2);
    Key_Schedule[10] = temp1;
}

int AES_set_encrypt_key(const uint8_t* userKey,
    const int bits,
    AES_KEY* key) {
    if (!userKey || !key)
        return -1;
    if (bits == 128)
    {
        AES_128_Key_Expansion(userKey, key->KEY);
        key->nr = 10;
        return 0;
    }
    return -2;
}


void AES_CTR_encrypt(const uint8_t* in,unsigned char* out,const uint8_t ivec[8],
    const uint8_t nonce[4],unsigned long length,const uint8_t* key,int number_of_rounds){
    __m128i ctr_block=_mm_set_epi64x(0,0), tmp, ONE, BSWAP_EPI64;
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


void test_AES_CTR() {
    uint8_t AES_TEST_VECTOR[32] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
        0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F };
    const uint8_t AES128_TEST_KEY[32] = { 0x7E,0x24,0x06,0x78,0x17,0xFA,0xE0,0xD7,
                                        0x43,0xD6,0xCE,0x1F,0x32,0x53,0x91,0x63 };
    uint8_t CTR128_EXPECTED[] = { 0x51,0x04,0xA1,0x06,0x16,0x8A,0x72,0xD9,
        0x79,0x0D,0x41,0xEE,0x8E,0xDA,0xD3,0x88,
        0xEB,0x2E,0x1E,0xFC,0x46,0xDA,0x57,0xC8,
        0xFC,0xE6,0x30,0xDF,0x91,0x41,0xBE,0x28 };
    int LENGTH = 64;
    uint8_t* PLAINTEXT = (uint8_t*)malloc(LENGTH);
    uint8_t* CIPHERTEXT = (uint8_t*)malloc(LENGTH);
    uint8_t* DECRYPTEDTEXT = (uint8_t*)malloc(LENGTH);
    int i, j;
    AES_KEY key;
    for (i = 0; i < LENGTH / 16 / 2; i++) {
        for (j = 0; j < 2; j++) {
            _mm_storeu_si128(&((__m128i*)PLAINTEXT)[i * 2 + j],
                *((__m128i*)AES_TEST_VECTOR + j));
        }
    }
    for (j = i * 2; j < LENGTH / 16; j++) {
        _mm_storeu_si128(&((__m128i*)PLAINTEXT)[j],
            ((__m128i*)AES_TEST_VECTOR)[j % 4]);
    }
    if (LENGTH % 16) {
        _mm_storeu_si128(&((__m128i*)PLAINTEXT)[j],
            ((__m128i*)AES_TEST_VECTOR)[j % 4]);
    }
    AES_set_encrypt_key(AES128_TEST_KEY, 128, &key);
    AES_CTR_encrypt(PLAINTEXT,CIPHERTEXT,CTR128_IV,CTR128_NONCE,LENGTH,key.KEY,key.nr);
    AES_CTR_encrypt(CIPHERTEXT, DECRYPTEDTEXT, CTR128_IV, CTR128_NONCE, LENGTH, key.KEY, key.nr);
}

std::string getCoeff(const uint8_t* key) {
    uint8_t iv[CryptoPP::Salsa20::IV_LENGTH] = { 0x00 };
    CryptoPP::Salsa20::Encryption salsaEncryption;
    salsaEncryption.SetKeyWithIV(key, CryptoPP::Salsa20::DEFAULT_KEYLENGTH, iv);
    std::string stringsource(CHALLENGE_NUM * sizeof(AES128_BLOCK_SIZE), 0x00);
    std::string ciphertext;
    CryptoPP::StringSource(stringsource, true,
        new CryptoPP::StreamTransformationFilter(salsaEncryption,
            new CryptoPP::StringSink(ciphertext)));
    return ciphertext;
}


std::vector<uint32_t> getRandomIndex(const uint8_t* key,uint32_t file_length) {
    uint8_t iv[CryptoPP::Salsa20::IV_LENGTH] = { 0x00 };
    CryptoPP::Salsa20::Encryption salsaEncryption;
    salsaEncryption.SetKeyWithIV(key, CryptoPP::Salsa20::DEFAULT_KEYLENGTH, iv);
    std::string stringsource(CHALLENGE_NUM * sizeof(uint32_t), 0x00);
    std::string ciphertext;
    CryptoPP::StringSource(stringsource, true,
        new CryptoPP::StreamTransformationFilter(salsaEncryption,
            new CryptoPP::StringSink(ciphertext)));
    std::vector<uint32_t> nums;
    uint32_t* num = (uint32_t*)stringsource.c_str();
    for (int i = 0; i < CHALLENGE_NUM; i++) {
        nums.push_back(*num%file_length);
        num++;
    }
    return nums;
}

void save_file(const uint8_t* data, std::size_t size, std::string file_name)
{
    std::ofstream outfile(file_name, std::ios::binary|std::ios::trunc);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(data), size);
        outfile.close();
        std::cout << "write successfully" << std::endl;
    }
    else {
        std::cerr << "can't open file" << std::endl;
    }
}

void save_key()
{
    std::ofstream outfile(key_path, std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<const char*>(key_A), AES128_BLOCK_SIZE);
        outfile.write(reinterpret_cast<const char*>(key_B), AES128_BLOCK_SIZE);
        outfile.write(reinterpret_cast<const char*>(s_a), AES128_BLOCK_SIZE);
        outfile.write(reinterpret_cast<const char*>(s_b), AES128_BLOCK_SIZE);
        outfile.write(reinterpret_cast<const char*>(key_digest_a), AES128_BLOCK_SIZE);
        outfile.write(reinterpret_cast<const char*>(key_digest_b), AES128_BLOCK_SIZE);

        outfile.close();
        std::cout << "save key_file successfully" << std::endl;
    }
    else {
        std::cerr << "can't open key_file" << std::endl;
    }
}

void read_key()
{
    std::ifstream infile(key_path, std::ios::binary);
    if (infile.is_open()) {
        infile.read(reinterpret_cast<char*>(key_A), AES128_BLOCK_SIZE);
        infile.read(reinterpret_cast<char*>(key_B), AES128_BLOCK_SIZE);
        infile.read(reinterpret_cast<char*>(s_a), AES128_BLOCK_SIZE);
        infile.read(reinterpret_cast<char*>(s_b), AES128_BLOCK_SIZE);
        infile.read(reinterpret_cast<char*>(key_digest_a), AES128_BLOCK_SIZE);
        infile.read(reinterpret_cast<char*>(key_digest_b), AES128_BLOCK_SIZE);

        infile.close();
        std::cout << "read key_file successfully" << std::endl;
    }
    else {
        std::cerr << "can't open key_file" << std::endl;
    }
}

