#include "gfmul.h"
#include "general.h"
#include "Client.h"





int main() {
	//__m128i a = { 0x95, 0x2b, 0x2a, 0x56, 0xa5, 0x60, 0x4a, 0xc0, 0xb3, 0x2b, 0x66, 0x56, 0xa0, 0x5b, 0x40, 0xb6 };
	//__m128i a_;
	//getInverseEle(a,a_);
	//__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	//__m128i num1 = { 0x97, 0x2b, 0x2a, 0x56, 0xa5, 0x60, 0x4a, 0xc0, 0xb3, 0x2b, 0x66, 0x56, 0xa0, 0x5b, 0x40, 0xb6 };
	//num1 = _mm_shuffle_epi8(num1, bswap_mask);
	//a = _mm_shuffle_epi8(a, bswap_mask);
	//a_ = _mm_shuffle_epi8(a_, bswap_mask);
	//__m128i result;
	//print_bytes((uint8_t*)&num1, 16);
	//gfmul_(num1, a, result);
	//print_bytes((uint8_t*)&result, 16);
	//gfmul_(result, a_, result);
	//result = _mm_shuffle_epi8(result, bswap_mask);
	//print_bytes((uint8_t*)&result, 16);
	Client client("user");
	client.KeyGen();
	client.outsource(s_a,key_A);
}