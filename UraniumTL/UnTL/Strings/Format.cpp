#include <UnTL/Strings/Format.h>
#include <UnTL/Strings/Internal/jeaiii_to_text.h>

UN_PUSH_MSVC_WARNING(4702)
#include <dragonbox/dragonbox_to_chars.h>
UN_POP_MSVC_WARNING

namespace UN::Fmt
{
    UN_FINLINE char* TrimEmptyExp(char* buffer, const char* begin)
    {
        if (buffer - begin < 2)
        {
            return buffer;
        }

        if (*(buffer - 1) == '0' && *(buffer - 2) == 'E')
        {
            return buffer - 2;
        }

        return buffer;
    }

    void ValueFormatter<Float32>::Format(String& buffer, const Float32& value) const
    {
        constexpr Int32 BufferLength = jkj::dragonbox::max_output_string_length<jkj::dragonbox::ieee754_binary32>;
        char buf[BufferLength];
        auto* ptr = TrimEmptyExp(jkj::dragonbox::to_chars_n(value, buf), buf);
        buffer.Append(buf, ptr - buf);
    }

    void ValueFormatter<Float64>::Format(String& buffer, const Float64& value) const
    {
        constexpr Int32 BufferLength = jkj::dragonbox::max_output_string_length<jkj::dragonbox::ieee754_binary64>;
        char buf[BufferLength];
        auto* ptr = TrimEmptyExp(jkj::dragonbox::to_chars_n(value, buf), buf);
        buffer.Append(buf, ptr - buf);
    }

    void FormatIntegral(String& buffer, UInt64 value)
    {
        constexpr Int32 BufferLength = 21;
        char buf[BufferLength];
        auto* ptr = jeaiii::to_text_from_integer(buf, value);
        buffer.Append(buf, ptr - buf);
    }

    void FormatIntegral(String& buffer, Int64 value)
    {
        constexpr Int32 BufferLength = 21;
        char buf[BufferLength];
        auto* ptr = jeaiii::to_text_from_integer(buf, value);
        buffer.Append(buf, ptr - buf);
    }
} // namespace UN::Fmt
