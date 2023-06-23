
#pragma once
#include <stdint.h>
#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include "general.h"
struct aes_block {
	uint64_t a;
	uint64_t b;
};
__m128i reflect_xmm(__m128i X);
void gfmul_(__m128i a, __m128i b, __m128i* res);
void gfmul_(__m128i a, __m128i b, __m128i& res);
void print_m128i_with_string(const char* string, __m128i data);
void getInverseEle(__m128i a, __m128i& res);
void test_gfmul();

