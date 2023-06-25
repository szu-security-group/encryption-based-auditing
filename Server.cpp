#include "Server.h"


Server::Server(std::string name) {
    name.copy(this->name, name.length());
}


void Server::response(chall t_chall,const std::string file_name, uint8_t*proof)
{
    std::ifstream infile(file_name, std::ios::in | std::ios::binary);
    uint32_t max = static_cast<uint32_t>(infile.tellg());
    std::vector<uint32_t> indices;
    indices = getRandomIndex(t_chall.index, max);
    std::string str_coeff;
    char buffer[AES128_BLOCK_SIZE];
    str_coeff = getCoeff(t_chall.coeff);
    __m128i* __128coeff = (__m128i*)str_coeff.c_str();
    __m128i sum  = _mm_set_epi32(0,0,0,0);
 
    for (int i = 0; i < CHALLENGE_NUM; i++) {
        infile.seekg(indices[i] * AES128_BLOCK_SIZE);
        infile.read(buffer, AES128_BLOCK_SIZE);
        __m128i tmp;
        gfmul_(*(__m128i*)buffer, *__128coeff, tmp);
        sum = _mm_xor_si128(sum, tmp);
    }
    _mm_store_si128((__m128i*)proof, sum);
};
