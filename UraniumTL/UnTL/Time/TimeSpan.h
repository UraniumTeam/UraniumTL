#pragma once
#include <UnTL/Base/Base.h>
#include <UnTL/Base/Platform.h>
#include <ctime>
#include <ostream>
#include <sstream>

namespace UN
{
    //! \brief Represents a time span: an interval between two instances of DateTime.
    class TimeSpan
    {
        tm m_Data;

        inline explicit TimeSpan(time_t time)
        {
            m_Data = *localtime(&time);
        }

    public:
        UN_RTTI_Class(TimeSpan, "F42DAA6C-53F3-4AA5-9971-9783D8754F6C");

        [[nodiscard]] inline Int32 Years() const
        {
            return m_Data.tm_year;
        }

        [[nodiscard]] inline Int32 Days() const
        {
            return m_Data.tm_mday;
        }

        [[nodiscard]] inline Int32 Hours() const
        {
            return m_Data.tm_hour;
        }

        [[nodiscard]] inline Int32 Minutes() const
        {
            return m_Data.tm_min;
        }

        [[nodiscard]] inline Int32 Seconds() const
        {
            auto s = m_Data.tm_sec;
            if (s == 60)
                return 0;
            return s;
        }

        [[nodiscard]] inline Int64 TotalSeconds() const
        {
            auto copy = m_Data;
            return mktime(&copy);
        }

        [[nodiscard]] inline static TimeSpan FromSeconds(time_t seconds)
        {
            return TimeSpan(seconds);
        }
    };
} // namespace UN
