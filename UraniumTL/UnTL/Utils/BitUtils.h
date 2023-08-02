#pragma once
#include <UnTL/Base/Base.h>

namespace UN::Bits
{
#ifdef UN_COMPILER_MSVC

    UN_FINLINE UInt32 CountTrailingZeros(UInt32 value) noexcept
    {
        unsigned long tz = 0;
        return _BitScanForward(&tz, value) ? tz : 32;
    }

    UN_FINLINE UInt32 CountLeadingZeros(UInt32 value) noexcept
    {
        unsigned long lz = 0;
        return _BitScanReverse(&lz, value) ? 31 - lz : 32;
    }

#else

    UN_FINLINE UInt32 CountTrailingZeros(UInt32 value) noexcept
    {
        return __builtin_ctz(value);
    }

    UN_FINLINE UInt32 CountLeadingZeros(UInt32 value) noexcept
    {
        return __builtin_clz(value);
    }

#endif
} // namespace UN
