#include <stdint.h>
#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <stdio.h>
#include "gfmul.h"

__m128i reflect_xmm(__m128i X)
{
	__m128i tmp2, tmp1;
	__m128i AND_MASK =
		_mm_set_epi32(0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f);
	__m128i LOWER_MASK =
		_mm_set_epi32(0x0f070b03, 0x0d050901, 0x0e060a02, 0x0c040800);
	__m128i HIGHER_MASK =
		_mm_set_epi32(0xf070b030, 0xd0509010, 0xe060a020, 0xc0408000);
	__m128i BSWAP_MASK =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	//__m128i BSWAP_MASK =
	//_mm_set_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
	tmp2 = _mm_srli_epi16(X, 4);
	tmp1 = _mm_and_si128(AND_MASK, X);
	tmp2 = _mm_and_si128(AND_MASK, tmp2);
	tmp1 = _mm_shuffle_epi8(HIGHER_MASK, tmp1);
	tmp2 = _mm_shuffle_epi8(LOWER_MASK, tmp2);
	tmp1 = _mm_xor_si128(tmp1, tmp2);
	tmp1 = _mm_shuffle_epi8(tmp1, BSWAP_MASK);
	return tmp1;
}

void gfmul_(__m128i a, __m128i b, __m128i& res) {
	__m128i tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
	tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
	tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
	tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
	tmp6 = _mm_clmulepi64_si128(a, b, 0x11);

	tmp4 = _mm_xor_si128(tmp4, tmp5);
	tmp5 = _mm_slli_si128(tmp4, 8);
	tmp4 = _mm_srli_si128(tmp4, 8);
	tmp3 = _mm_xor_si128(tmp3, tmp5);
	tmp6 = _mm_xor_si128(tmp6, tmp4);

	tmp7 = _mm_srli_epi32(tmp3, 31);
	tmp8 = _mm_srli_epi32(tmp6, 31);
	tmp3 = _mm_slli_epi32(tmp3, 1);
	tmp6 = _mm_slli_epi32(tmp6, 1);

	tmp9 = _mm_srli_si128(tmp7, 12);
	tmp8 = _mm_slli_si128(tmp8, 4);
	tmp7 = _mm_slli_si128(tmp7, 4);
	tmp3 = _mm_or_si128(tmp3, tmp7);
	tmp6 = _mm_or_si128(tmp6, tmp8);
	tmp6 = _mm_or_si128(tmp6, tmp9);

	tmp7 = _mm_slli_epi32(tmp3, 31);
	tmp8 = _mm_slli_epi32(tmp3, 30);
	tmp9 = _mm_slli_epi32(tmp3, 25);

	tmp7 = _mm_xor_si128(tmp7, tmp8);
	tmp7 = _mm_xor_si128(tmp7, tmp9);
	tmp8 = _mm_srli_si128(tmp7, 4);
	tmp7 = _mm_slli_si128(tmp7, 12);
	tmp3 = _mm_xor_si128(tmp3, tmp7);

	tmp2 = _mm_srli_epi32(tmp3, 1);
	tmp4 = _mm_srli_epi32(tmp3, 2);
	tmp5 = _mm_srli_epi32(tmp3, 7);
	tmp2 = _mm_xor_si128(tmp2, tmp4);
	tmp2 = _mm_xor_si128(tmp2, tmp5);
	tmp2 = _mm_xor_si128(tmp2, tmp8);
	tmp3 = _mm_xor_si128(tmp3, tmp2);
	tmp6 = _mm_xor_si128(tmp6, tmp3);
	res = tmp6;
}

void gfmul_(__m128i a, __m128i b, __m128i* res) {
	__m128i tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
	tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
	tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
	tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
	tmp6 = _mm_clmulepi64_si128(a, b, 0x11);

	tmp4 = _mm_xor_si128(tmp4, tmp5);
	tmp5 = _mm_slli_si128(tmp4, 8);
	tmp4 = _mm_srli_si128(tmp4, 8);
	tmp3 = _mm_xor_si128(tmp3, tmp5);
	tmp6 = _mm_xor_si128(tmp6, tmp4);

	tmp7 = _mm_srli_epi32(tmp3, 31);
	tmp8 = _mm_srli_epi32(tmp6, 31);
	tmp3 = _mm_slli_epi32(tmp3, 1);
	tmp6 = _mm_slli_epi32(tmp6, 1);

	tmp9 = _mm_srli_si128(tmp7, 12);
	tmp8 = _mm_slli_si128(tmp8, 4);
	tmp7 = _mm_slli_si128(tmp7, 4);
	tmp3 = _mm_or_si128(tmp3, tmp7);
	tmp6 = _mm_or_si128(tmp6, tmp8);
	tmp6 = _mm_or_si128(tmp6, tmp9);

	tmp7 = _mm_slli_epi32(tmp3, 31);
	tmp8 = _mm_slli_epi32(tmp3, 30);
	tmp9 = _mm_slli_epi32(tmp3, 25);

	tmp7 = _mm_xor_si128(tmp7, tmp8);
	tmp7 = _mm_xor_si128(tmp7, tmp9);
	tmp8 = _mm_srli_si128(tmp7, 4);
	tmp7 = _mm_slli_si128(tmp7, 12);
	tmp3 = _mm_xor_si128(tmp3, tmp7);

	tmp2 = _mm_srli_epi32(tmp3, 1);
	tmp4 = _mm_srli_epi32(tmp3, 2);
	tmp5 = _mm_srli_epi32(tmp3, 7);
	tmp2 = _mm_xor_si128(tmp2, tmp4);
	tmp2 = _mm_xor_si128(tmp2, tmp5);
	tmp2 = _mm_xor_si128(tmp2, tmp8);
	tmp3 = _mm_xor_si128(tmp3, tmp2);
	tmp6 = _mm_xor_si128(tmp6, tmp3);
	*res = tmp6;
}

void getInverseEle(__m128i a, __m128i& res) {
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	a = _mm_shuffle_epi8(a, bswap_mask);
	res = _mm_set_epi8(0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	for (int i = 1;i< 128; i++) {
		gfmul_(a, a, a);
		gfmul_(a, res, res);
	}
	res = _mm_shuffle_epi8(res,bswap_mask);
}

void print_m128i_with_string(const char* string, __m128i data)
{
	unsigned char* pointer = (unsigned char*)&data;
	int i;
	printf("%-20s[0x", string);
	for (i = 0; i < 16; i++)
		printf("%02x", pointer[i]);
	printf("]\n");
}


void test_gfmul()
{
	// test case 1 and 2 from Intel® carry-less multiplication instruction and its usage for computing the GCM mode in page 71
	// Gueron, S., & Kounavis, M. E. (2010). Intel® carry-less multiplication instruction and its usage for computing the GCM mode. White Paper, 10.
	// URL : https://www.intel.com/content/dam/develop/external/us/en/documents/clmul-wp-rev-2-02-2014-04-20.pdf
	/*
	// test case 1 start
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i num1 = { 0x7b, 0x5b, 0x54, 0x65, 0x73, 0x74, 0x56, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x5d, 0x53, 0x47, 0x5d };
	__m128i num2 = { 0x48, 0x69, 0x28, 0x53, 0x68, 0x61, 0x79, 0x29, 0x5b, 0x47, 0x75, 0x65, 0x72, 0x6f, 0x6e, 0x5d };
	// expected result = {0x04, 0x02, 0x29, 0xa0, 0x9a, 0x5e, 0xd1, 0x2e, 0x7e, 0x4e, 0x10, 0xda, 0x32, 0x35, 0x06, 0xd2}
	__m128i result;
	num1 = _mm_shuffle_epi8(num1, bswap_mask);
	num2 = _mm_shuffle_epi8(num2, bswap_mask);
	num1 = reflect_xmm(num1);
	num2 = reflect_xmm(num2);
	gfmul_(num1, num2, &result);
	result = reflect_xmm(result);
	result = _mm_shuffle_epi8(result, bswap_mask);
	print_m128i_with_string("result : ", result);
	// test case 1 end
	*/
	/*
	// test case 2 start
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i num1 = { 0x95, 0x2b, 0x2a, 0x56, 0xa5, 0x60, 0x4a, 0xc0, 0xb3, 0x2b, 0x66, 0x56, 0xa0, 0x5b, 0x40, 0xb6};
	__m128i num2 = { 0xdf, 0xa6, 0xbf, 0x4d, 0xed, 0x81, 0xdb, 0x03, 0xff, 0xca, 0xff, 0x95, 0xf8, 0x30, 0xf0, 0x61};
	// expected result = {0xda, 0x53, 0xeb, 0x0a, 0xd2, 0xc5, 0x5b, 0xb6, 0x4f, 0xc4, 0x80, 0x2c, 0xc3, 0xfe, 0xda, 0x60}
	__m128i result;
	num1 = _mm_shuffle_epi8(num1, bswap_mask);
	num2 = _mm_shuffle_epi8(num2, bswap_mask);
	gfmul_(num1, num2, &result);
	result = _mm_shuffle_epi8(result, bswap_mask);
	print_m128i_with_string("result : ", result);
	// test case 2 end
	*/
	// test case 3 from The Galois/counter mode of operation (GCM) in page 27
	// McGrew, D., & Viega, J. (2004). The Galois/counter mode of operation (GCM). submission to NIST Modes of Operation Process, 20, 0278-0070.
	// URL : https://csrc.nist.rip/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
	/*
	// test case 3 start
	__m128i bswap_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	__m128i num1 = { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92, 0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 };
	__m128i num2 = { 0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b, 0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e };
	// expected result = {0x5e, 0x2e, 0xc7, 0x46, 0x91, 0x70, 0x62, 0x88, 0x2c, 0x85, 0xb0, 0x68, 0x53, 0x53, 0xde, 0xb7}
	__m128i result;
	num1 = _mm_shuffle_epi8(num1, bswap_mask);
	num2 = _mm_shuffle_epi8(num2, bswap_mask);
	gfmul_(num1, num2, &result);
	result = _mm_shuffle_epi8(result, bswap_mask);
	print_m128i_with_string("result : ", result);
	// test case 3 end
	*/
}
