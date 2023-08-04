#pragma once
#include <UnTL/Base/Base.h>
#include <UnTL/Strings/String.h>
#include <UnTL/Strings/Unicode.h>
#include <array>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string_view>

namespace UN::Fmt
{
    using UN::String;
    using UN::StringSlice;

    //! \brief Interface for value formatters.
    //!
    //! \note The interface is for internal usage only.
    struct IValueFormatter
    {
        //! \internal
        virtual void Format(String& buffer, void* value) const = 0;
    };

    //! \brief Base value formatter.
    template<class T>
    struct BasicValueFormatter : IValueFormatter
    {
        virtual void Format(String& buffer, const T& value) const = 0;

        //! \brief Format a specified value.
        //!
        //! \param buffer - The buffer to write the formatted text to.
        //! \param value - The value to format.
        void Format(String& buffer, void* value) const final
        {
            Format(buffer, *reinterpret_cast<T*>(value));
        }
    };

    void FormatIntegral(String& buffer, UInt64 value);
    void FormatIntegral(String& buffer, Int64 value);

    template<class T>
    struct ValueFormatter : BasicValueFormatter<T>
    {
        void Format(String& buffer, const T& value) const override
        {
            std::stringstream ss;
            if constexpr (std::is_pointer_v<T> && !std::is_same_v<T, const char*>)
            {
                ss << "0x" << std::hex << reinterpret_cast<USize>(value);
            }
            else if constexpr (std::is_signed_v<T>)
            {
                FormatIntegral(buffer, static_cast<Int64>(value));
                return;
            }
            else if constexpr (std::is_unsigned_v<T>)
            {
                FormatIntegral(buffer, static_cast<UInt64>(value));
                return;
            }
            else
            {
                ss << value;
            }

            auto v = ss.str();
            buffer.Append(v.data(), v.size());
        }
    };

    template<>
    struct ValueFormatter<Float32> : BasicValueFormatter<Float32>
    {
        void Format(String& buffer, const Float32& value) const override;
    };

    template<>
    struct ValueFormatter<Float64> : BasicValueFormatter<Float64>
    {
        void Format(String& buffer, const Float64& value) const override;
    };

    template<>
    struct ValueFormatter<String> : BasicValueFormatter<String>
    {
        void Format(String& buffer, const String& value) const override
        {
            buffer.Append(value);
        }
    };

    template<>
    struct ValueFormatter<StringSlice> : BasicValueFormatter<StringSlice>
    {
        void Format(String& buffer, const StringSlice& value) const override
        {
            buffer.Append(value);
        }
    };

    template<>
    struct ValueFormatter<std::string> : BasicValueFormatter<std::string>
    {
        void Format(String& buffer, const std::string& value) const override
        {
            buffer.Append(value.data(), value.size());
        }
    };

    template<>
    struct ValueFormatter<std::string_view> : BasicValueFormatter<std::string_view>
    {
        void Format(String& buffer, const std::string_view& value) const override
        {
            buffer.Append(value.data(), value.size());
        }
    };

    template<>
    struct ValueFormatter<UUID> : BasicValueFormatter<UUID>
    {
        void Format(String& buffer, const UUID& value) const override
        {
            static char digits[] = "0123456789ABCDEF";
            Int32 idx            = 0;
            buffer.Reserve(buffer.Size() + 36);
            auto append = [&](UInt32 n) {
                for (UInt32 i = 0; i < n; ++i)
                {
                    UInt8 c = value.Data[idx++];
                    buffer.Append(digits[(c & 0xF0) >> 4]);
                    buffer.Append(digits[(c & 0x0F) >> 0]);
                }
            };

            append(4);
            buffer.Append('-');
            append(2);
            buffer.Append('-');
            append(2);
            buffer.Append('-');
            append(2);
            buffer.Append('-');
            append(6);
        }
    };

    namespace Internal
    {
        template<class TFormatter>
        inline TFormatter* GetFormatter()
        {
            static TFormatter instance;
            return &instance;
        }

        struct FormatArg
        {
            inline FormatArg() = default;

            inline FormatArg(void* value, IValueFormatter* formatter) noexcept
            {
                Value     = value;
                Formatter = formatter;
            }

            template<class T>
            inline static FormatArg Create(T&& arg) noexcept
            {
                auto* f = GetFormatter<ValueFormatter<std::decay_t<T>>>();
                return FormatArg((void*)&arg, f);
            }

            inline void FormatTo(String& str) const
            {
                Formatter->Format(str, Value);
            }

            void* Value                = nullptr;
            IValueFormatter* Formatter = nullptr;
        };

        template<size_t ArgsCount>
        struct FormatArgs
        {
            std::array<FormatArg, ArgsCount> Data;

            [[nodiscard]] inline bool HasUnused() const
            {
                for (const auto& arg : Data)
                    if (arg.Value)
                        return true;
                return false;
            }
        };

        template<size_t ArgsCount>
        void FormatImpl(String& str, StringSlice fmt, FormatArgs<ArgsCount>& args)
        {
            size_t argIndex = 0;
            auto begin      = fmt.begin();
            for (auto it = fmt.begin(); it != fmt.end(); ++it)
            {
                if (*it == '{')
                {
                    auto braceIt = it;
                    if (*++it == '{')
                    {
                        str.Append(StringSlice(begin, it));
                        begin = it;
                        begin++;
                        continue;
                    }

                    UN_Assert(*it == '}', ""); // do not accept arguments and indices for now
                    str.Append(StringSlice(begin, braceIt));
                    begin = it;
                    begin++;

                    auto& arg = args.Data[argIndex++];
                    arg.FormatTo(str);
                    arg.Value = nullptr;
                }
                else if (*it == '}')
                {
                    ++it;
                    UN_Assert(*it == '}', "must be escaped");
                    str.Append(StringSlice(begin, it));
                    begin = it;
                    begin++;
                    continue;
                }
            }
            str.Append(StringSlice(begin, fmt.end()));
        }
    } // namespace Internal

    //! \brief Format a string with arguments.
    //!
    //! The function implements python-like string formatting, i.e. every occurrence of '{}'
    //! is replaced by the corresponding format argument.\n
    //!
    //! Example:
    //! \code{.cpp}
    //!     auto string = Format("{} + {} = {}", 2, 2, 5); // "2 + 2 = 5"
    //! \endcode
    //!
    //! This function will use an implementation of ValueFormatter<T> to format an argument of type T.
    //!
    //! \tparam Args - Types of the format arguments.
    //!
    //! \param fmt - The format string.
    //! \param args - The format arguments.
    //! \return The formatted string.
    template<class... Args>
    inline String Format(StringSlice fmt, Args&&... args)
    {
        String result;
        Internal::FormatArgs<sizeof...(Args)> formatArgs{ Internal::FormatArg::Create(args)... };
        Internal::FormatImpl<sizeof...(Args)>(result, fmt, formatArgs);
        return result;
    }
} // namespace UN::Fmt
