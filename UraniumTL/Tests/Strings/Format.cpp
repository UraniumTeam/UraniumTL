#include <UnTL/Strings/Format.h>
#include <gtest/gtest.h>

using namespace UN;

TEST(Format, Escape)
{
    EXPECT_EQ(Fmt::Format("qqq{{"), "qqq{");
    EXPECT_EQ(Fmt::Format("{{qqq"), "{qqq");
    EXPECT_EQ(Fmt::Format("{{"), "{");
    EXPECT_EQ(Fmt::Format("qqq{{qqq"), "qqq{qqq");

    EXPECT_EQ(Fmt::Format("qqq}}"), "qqq}");
    EXPECT_EQ(Fmt::Format("}}qqq"), "}qqq");
    EXPECT_EQ(Fmt::Format("}}"), "}");
    EXPECT_EQ(Fmt::Format("qqq}}qqq"), "qqq}qqq");

    EXPECT_EQ(Fmt::Format("{{}}"), "{}");
    EXPECT_EQ(Fmt::Format("}}{{"), "}{");
    EXPECT_EQ(Fmt::Format("}}}}"), "}}");
    EXPECT_EQ(Fmt::Format("{{{{"), "{{");

    EXPECT_EQ(Fmt::Format("{{{}}}", 0), "{0}");
}

TEST(Format, Float)
{
    EXPECT_EQ(Fmt::Format("{}", 1.0), "1");
    EXPECT_EQ(Fmt::Format("{}", 1.0f), "1");
    EXPECT_EQ(Fmt::Format("{}", 1.234), "1.234");
    EXPECT_EQ(Fmt::Format("{}", 1.234f), "1.234");
}

TEST(Format, Integer)
{
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int8>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<UInt8>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int16>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<UInt16>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int32>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<UInt32>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int64>(123)), "123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<UInt64>(123)), "123");

    EXPECT_EQ(Fmt::Format("{}", static_cast<Int8>(-123)), "-123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int16>(-123)), "-123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int32>(-123)), "-123");
    EXPECT_EQ(Fmt::Format("{}", static_cast<Int64>(-123)), "-123");

    EXPECT_EQ(Fmt::Format("{}", std::numeric_limits<Int64>::min()), "-9223372036854775808");
    EXPECT_EQ(Fmt::Format("{}", std::numeric_limits<Int64>::max()), "9223372036854775807");
    EXPECT_EQ(Fmt::Format("{}", std::numeric_limits<UInt64>::min()), "0");
    EXPECT_EQ(Fmt::Format("{}", std::numeric_limits<UInt64>::max()), "18446744073709551615");
}

TEST(Format, UUID)
{
    UUID uuid("62e1b7a1-c14a-4129-ac57-7e77289123e9");
    EXPECT_EQ(Fmt::Format("{}", uuid), "62E1B7A1-C14A-4129-AC57-7E77289123E9");
    EXPECT_EQ(Fmt::Format("Asset loader for `{}` not found", uuid),
              "Asset loader for `62E1B7A1-C14A-4129-AC57-7E77289123E9` not found");
}

TEST(Format, Strings)
{
    // EXPECT_EQ(Fmt::Format("{}", "literal"), "literal");
    EXPECT_EQ(Fmt::Format("{}", StringSlice("slice")), "slice");
    EXPECT_EQ(Fmt::Format("{}", String("str")), String("str"));
    EXPECT_EQ(Fmt::Format("{}", String("loooooooooooooooooooooooooooooong str")),
              String("loooooooooooooooooooooooooooooong str"));
}
