#include <UnTL/Utils/UUID.h>
#include <emmintrin.h>
#include <immintrin.h>

namespace UN::Internal
{
    // derived from https://github.com/crashoz/uuid_v4

#if UN_SSE41_SUPPORTED && UN_AVX2_SUPPORTED
    void m128itos(__m128i x, char* mem)
    {
        // Expand each byte in x to two bytes in res
        // i.e. 0x12345678 -> 0x0102030405060708
        // Then translate each byte to its hex ascii representation
        // i.e. 0x0102030405060708 -> 0x3132333435363738
        const __m256i mask         = _mm256_set1_epi8(0x0F);
        const __m256i add          = _mm256_set1_epi8(0x06);
        const __m256i alpha_mask   = _mm256_set1_epi8(0x10);
        const __m256i alpha_offset = _mm256_set1_epi8(0x37);

        __m256i a      = _mm256_castsi128_si256(x);
        __m256i as     = _mm256_srli_epi64(a, 4);
        __m256i lo     = _mm256_unpacklo_epi8(as, a);
        __m128i hi     = _mm256_castsi256_si128(_mm256_unpackhi_epi8(as, a));
        __m256i c      = _mm256_inserti128_si256(lo, hi, 1);
        __m256i d      = _mm256_and_si256(c, mask);
        __m256i alpha  = _mm256_slli_epi64(_mm256_and_si256(_mm256_add_epi8(d, add), alpha_mask), 3);
        __m256i offset = _mm256_blendv_epi8(_mm256_slli_epi64(add, 3), alpha_offset, alpha);
        __m256i res    = _mm256_add_epi8(d, offset);

        // Add dashes between blocks as specified in RFC-4122
        // 8-4-4-4-12
        const __m256i dash_shuffle =
            _mm256_set_epi32(0x0b0a0908, 0x07060504, 0x80030201, 0x00808080, 0x0d0c800b, 0x0a090880, 0x07060504, 0x03020100);
        const __m256i dash =
            _mm256_set_epi64x(0x0000000000000000ull, 0x2d000000002d0000ull, 0x00002d000000002d, 0x0000000000000000ull);

        __m256i resd = _mm256_shuffle_epi8(res, dash_shuffle);
        resd         = _mm256_or_si256(resd, dash);

        _mm256_storeu_si256((__m256i*)mem, resd);
        *(uint16_t*)(mem + 16) = (uint16_t)_mm256_extract_epi16(res, 7);
        *(uint32_t*)(mem + 32) = _mm256_extract_epi32(res, 7);
    }
#endif

    __m128i m128gen(std::mt19937_64& generator)
    {
        // These are SSE2 instructions, so we don't need to check for support

        std::uniform_int_distribution<UInt64> distribution(std::numeric_limits<UInt64>::min(),
                                                           std::numeric_limits<UInt64>::max());
        const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
        const __m128i or_mask  = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
        __m128i n              = _mm_set_epi64x(distribution(generator), distribution(generator));
        return _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);
    }
} // namespace UN::Internal
