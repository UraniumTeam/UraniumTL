#pragma once
#include <UnTL/Base/Base.h>
#include <array>
#include <cctype>
#include <ostream>
#include <random>
#include <string_view>

namespace UN
{
    namespace Internal
    {
        inline constexpr USize UUIDStringLength = 36;

#if UN_SSE41_SUPPORTED && UN_AVX2_SUPPORTED
        void m128itos(__m128i x, char* mem);
#endif
        __m128i m128gen(std::mt19937_64& generator);
    } // namespace Internal

    //! \brief A struct to work with UUIDs.
    struct alignas(__m128i) UUID
    {
        std::array<UInt8, 16> Data{};

        inline UUID() = default;

        inline explicit UUID(__m128i value) noexcept
        {
            _mm_store_si128(reinterpret_cast<__m128i*>(Data.data()), value);
        }

        inline UUID(const UUID& other) noexcept
        {
            __m128i value = _mm_load_si128(reinterpret_cast<const __m128i*>(other.Data.data()));
            _mm_store_si128(reinterpret_cast<__m128i*>(Data.data()), value);
        }

        inline UUID& operator=(const UUID& other) noexcept
        {
            __m128i value = _mm_load_si128(reinterpret_cast<const __m128i*>(other.Data.data()));
            _mm_store_si128(reinterpret_cast<__m128i*>(Data.data()), value);
            return *this;
        }

        //! \brief Parse a UUID from a string in form `"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"`.
        inline explicit UUID(const char* str) noexcept
        {
            [[maybe_unused]] auto result = TryParse(str, *this, false);
            UN_Assert(result, "Invalid format");
        }

        inline static UUID Create()
        {
            std::random_device device;
            std::mt19937_64 generator(device());
            __m128i value = Internal::m128gen(generator);
            return UUID(value);
        }

        inline static UUID Create(UInt64 seed)
        {
            std::mt19937_64 generator(seed);
            __m128i value = Internal::m128gen(generator);
            return UUID(value);
        }

        inline static bool TryParse(const char* str, UUID& result, bool assertLength = true)
        {
            static char digits[]    = "0123456789ABCDEF";
            constexpr auto getValue = [](char c) {
                return static_cast<UInt8>(std::find(digits, digits + 16, std::toupper(c)) - digits);
            };

            USize idx  = 0;
            auto parse = [&](Int32 n) -> bool {
                for (Int32 i = 0; i < n; ++i)
                {
                    auto v1 = getValue(*str++);
                    if (v1 >= 16)
                        return false;

                    auto v2 = getValue(*str++);
                    if (v2 >= 16)
                        return false;

                    result.Data[idx] = v1 << 4;
                    result.Data[idx++] |= v2 & 0x0F;
                }

                return true;
            };

            // clang-format off
            if (!parse(4))       return false;
            if (*str++ != '-')   return false;
            if (!parse(2))       return false;
            if (*str++ != '-')   return false;
            if (!parse(2))       return false;
            if (*str++ != '-')   return false;
            if (!parse(2))       return false;
            if (*str++ != '-')   return false;
            if (!parse(6))       return false;
            // clang-format on

            if (*str != '\0' && assertLength)
                return false;

            return true;
        }

        [[nodiscard]] inline std::string ToString() const
        {
            std::string buffer;

#if UN_SSE41_SUPPORTED && UN_AVX2_SUPPORTED
            buffer.resize(Internal::UUIDStringLength);
            Internal::m128itos(_mm_load_si128(reinterpret_cast<const __m128i*>(Data.data())), buffer.data());
#else
            static char digits[] = "0123456789ABCDEF";
            Int32 idx            = 0;
            buffer.reserve(Internal::UUIDStringLength);
            auto append = [&](UInt32 n) {
                for (UInt32 i = 0; i < n; ++i)
                {
                    UInt8 c = Data[idx++];
                    buffer.push_back(digits[(c & 0xF0) >> 4]);
                    buffer.push_back(digits[(c & 0x0F) >> 0]);
                }
            };

            append(4);
            buffer.push_back('-');
            append(2);
            buffer.push_back('-');
            append(2);
            buffer.push_back('-');
            append(2);
            buffer.push_back('-');
            append(6);
#endif

            return buffer;
        }
    };

    inline bool operator>(const UUID& lhs, const UUID& rhs)
    {
        auto* l = reinterpret_cast<const UInt64*>(lhs.Data.data());
        auto* r = reinterpret_cast<const UInt64*>(rhs.Data.data());

        if (l[0] != r[0])
        {
            return l[0] > r[0];
        }

        return l[1] > r[1];
    }

    inline bool operator<(const UUID& lhs, const UUID& rhs)
    {
        auto* l = reinterpret_cast<const UInt64*>(lhs.Data.data());
        auto* r = reinterpret_cast<const UInt64*>(rhs.Data.data());

        if (l[0] != r[0])
        {
            return l[0] < r[0];
        }

        return l[1] < r[1];
    }

    inline bool operator==(const UUID& lhs, const UUID& rhs)
    {
        return std::equal(lhs.Data.begin(), lhs.Data.end(), rhs.Data.begin());
    }

    inline bool operator!=(const UUID& lhs, const UUID& rhs)
    {
        return !(lhs == rhs);
    }
} // namespace UN

namespace std
{
    //!\ brief Calculate hash of a UN::UUID.
    template<>
    struct hash<UN::UUID>
    {
        inline size_t operator()(const UN::UUID& value) const noexcept
        {
            return hash<string_view>{}(string_view(reinterpret_cast<const char*>(value.Data.data()), value.Data.size()));
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const ::UN::UUID& uuid)
    {
        stream << uuid.ToString();
        return stream;
    }
} // namespace std
