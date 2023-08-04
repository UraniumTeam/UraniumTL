#include <UnTL/Strings/String.h>
#include <gtest/gtest.h>

using namespace UN;

TEST(Strings, AssignmentFreesMemory)
{
    auto& alloc = *SystemAllocator::Get();
    auto before = alloc.AllocationCount();
    {
        String str = "loooooooooooooooooooooooooooooooooooooooooong123";
        str        = String("loooooooooooooooooooooooooooooooooooooooooong321");
    }
    EXPECT_EQ(before, alloc.AllocationCount());
    {
        String str1 = "loooooooooooooooooooooooooooooooooooooooooong123";
        String str2 = "loooooooooooooooooooooooooooooooooooooooooong321";
        str2        = str1;
    }
    EXPECT_EQ(before, alloc.AllocationCount());
    {
        String str1 = "loooooooooooooooooooooooooooooooooooooooooong123";
        String str2 = String("loooooooooooooooooooooooooooooooooooooooooong321");
        str2        = str1;
    }
    EXPECT_EQ(before, alloc.AllocationCount());
    {
        String str1 = "loooooooooooooooooooooooooooooooooooooooooong123";
        String str2 = String("loooooooooooooooooooooooooooooooooooooooooong321");
        str2        = std::move(str1);
    }
    EXPECT_EQ(before, alloc.AllocationCount());
}

TEST(Strings, EmptySizeCapacity)
{
    String str;
    ASSERT_EQ(str.Capacity(), 22);
    ASSERT_EQ(str.Size(), 0);
}

TEST(Strings, SmallSizeCapacity)
{
    String str = "q";
    ASSERT_EQ(str.Capacity(), 22);
    ASSERT_EQ(str.Size(), 1);
}

TEST(Strings, LongSizeCapacity)
{
    const char* cstr = "loooooooooooooooooooooooooooooooooooooooooong";
    String str       = cstr;
    ASSERT_GE(str.Capacity(), 35);
    ASSERT_EQ(str.Size(), strlen(cstr));
}

TEST(Strings, SmallByteAt)
{
    String str = "0123456789";
    EXPECT_EQ(str.ByteAt(0), '0');
    EXPECT_EQ(str.ByteAt(9), '9');
}

TEST(Strings, LongByteAt)
{
    String str = "loooooooooooooooooooooooooooooooooooong 0123456789";
    EXPECT_EQ(str.ByteAt(40), '0');
    EXPECT_EQ(str.ByteAt(49), '9');
}

TEST(Strings, Length)
{
    String smalls = "0123";
    String longs  = "loooooooooooooooooooooooooooooooooooong";
    EXPECT_EQ(smalls.Length(), 4);
    EXPECT_EQ(longs.Length(), 39);
}

TEST(Strings, SmallCodepointAt)
{
    const char* utf8 = "qЯwgЫЧ";
    ASSERT_TRUE(UTF8::Valid(utf8));

    String str = utf8;
    EXPECT_EQ(str.CodePointAt(0), L'q');
    EXPECT_EQ(str.CodePointAt(3), L'g');
    // EXPECT_EQ(str.CodePointAt(4), L'Ы');
}

TEST(Strings, LongCodepointAt)
{
    const char* utf8 = "loooooooooooooooooooooooooooooooooooong qЯwgЫЧ";
    ASSERT_TRUE(UTF8::Valid(utf8));

    String str = utf8;
    EXPECT_EQ(str.CodePointAt(40), L'q');
#if !defined UN_COMPILER_MSVC
    EXPECT_EQ(str.CodePointAt(44), L'Ы');
#endif
}

TEST(Strings, Equals)
{
    String a = "abc";
    String b = "abc";
    String c = "xyz";
    String d = "qqqq";
    String e = "";
    String f;

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
    EXPECT_EQ(e, f);
    EXPECT_NE(a, e);
    EXPECT_NE(d, f);
}

TEST(Strings, Slice)
{
    String str = "===slice===";
    ASSERT_EQ(str(3, 8), "slice");
}

TEST(Strings, SmallConcat)
{
    String a = "A";
    String b = "B";
    ASSERT_EQ(a + b, "AB");
}

TEST(Strings, LongConcat)
{
    String a(128, 'A');
    String b(128, 'B');
    auto c = a + b;
    ASSERT_EQ(c(0, 128), a);
    ASSERT_EQ(c(128, 256), b);
}

TEST(Strings, ShrinkReserve)
{
    String str;
    EXPECT_EQ(str.Capacity(), 22); // initially small

    str.Reserve(128); // small -> long
    EXPECT_GE(str.Capacity(), 128);

    str.Append("123");
    str.Shrink(); // long -> small
    EXPECT_EQ(str.Capacity(), 22);

    const char l[] = "looooooooooooooooooooooooooooooooooooong";
    str.Append(l); // small -> long
    str.Shrink();  // long -> long
    EXPECT_GE(str.Capacity(), sizeof(l) + 3 - 1);
}

TEST(Strings, Compare)
{
    EXPECT_EQ(String{}.Compare(String{}), 0);

    EXPECT_EQ(String("abc").Compare(StringSlice("abc")), 0);
    EXPECT_EQ(String("abc").Compare(String("abc")), 0);

    EXPECT_EQ(String("abc").Compare("abc"), 0);

    EXPECT_GT(String("abcd").Compare("abc"), 0);
    EXPECT_LT(String("abc").Compare("abcd"), 0);

    EXPECT_GT(String("az").Compare("aa"), 0);
    EXPECT_LT(String("aa").Compare("az"), 0);

    EXPECT_GT(String("azz").Compare("aa"), 0);
    EXPECT_GT(String("az").Compare("aaz"), 0);
    EXPECT_GT(String("aza").Compare("aa"), 0);
    EXPECT_GT(String("az").Compare("aaa"), 0);
}

TEST(Strings, Split)
{
    auto str   = String("abc def 123");
    auto split = str.Split();
    ASSERT_EQ(split.Size(), 3);
    EXPECT_EQ(split[0], StringSlice("abc"));
    EXPECT_EQ(split[1], StringSlice("def"));
    EXPECT_EQ(split[2], StringSlice("123"));
}

TEST(Strings, SplitSpace)
{
    auto str   = String(" ");
    auto split = str.Split();
    ASSERT_EQ(split.Size(), 1);
    EXPECT_EQ(split[0], StringSlice{});
}

TEST(Strings, SplitEmpty)
{
    auto str   = String("");
    auto split = str.Split();
    ASSERT_EQ(split.Size(), 0);
}

TEST(Strings, SplitLines)
{
    auto str   = String("abc\ndef\r\n123");
    auto split = str.SplitLines();
    ASSERT_EQ(split.Size(), 3);
    EXPECT_EQ(split[0], StringSlice("abc"));
    EXPECT_EQ(split[1], StringSlice("def"));
    EXPECT_EQ(split[2], StringSlice("123"));
}

TEST(Strings, Strip)
{
    EXPECT_EQ(String("123").Strip(), StringSlice("123"));
    EXPECT_EQ(String("    123    ").StripRight(), StringSlice("    123"));
    EXPECT_EQ(String("    123    ").StripLeft(), StringSlice("123    "));
    EXPECT_EQ(String(" \t \n \r   ").StripLeft(), StringSlice{});
}

TEST(Strings, StartsWith)
{
    EXPECT_TRUE(String("").StartsWith(""));
    EXPECT_TRUE(String("1234").StartsWith(""));
    EXPECT_TRUE(String("1234").StartsWith("1"));
    EXPECT_TRUE(String("1234").StartsWith("12"));
    EXPECT_TRUE(String("1234").StartsWith("1234"));
    EXPECT_FALSE(String("1234").StartsWith("21"));
    EXPECT_FALSE(String("1234").StartsWith("12345"));
}

TEST(Strings, EndsWith)
{
    EXPECT_TRUE(String("").EndsWith(""));
    EXPECT_TRUE(String("1234").EndsWith(""));
    EXPECT_TRUE(String("1234").EndsWith("4"));
    EXPECT_TRUE(String("1234").EndsWith("34"));
    EXPECT_TRUE(String("1234").EndsWith("1234"));
    EXPECT_FALSE(String("1234").EndsWith("21"));
    EXPECT_FALSE(String("1234").EndsWith("12345"));
}

TEST(Strings, ParseErrors)
{
#define EXPECT_PARSE_ERR(expr, code, pos) EXPECT_EQ(expr, ::ParseError(code, pos))

    EXPECT_EQ(String("1").Parse<int>().Unwrap(), 1);
    EXPECT_EQ(String("-2").Parse<int>().Unwrap(), -2);
    EXPECT_EQ(String("3").Parse<unsigned>().Unwrap(), 3);
    EXPECT_EQ(String("1.5").Parse<float>().Unwrap(), 1.5f);
    EXPECT_EQ(String("-1.5").Parse<float>().Unwrap(), -1.5f);
    EXPECT_EQ(String("false").Parse<bool>().Unwrap(), false);
    EXPECT_EQ(String("true").Parse<bool>().Unwrap(), true);
    EXPECT_PARSE_ERR(String("").Parse<int>().UnwrapErr(), ParseErrorCode::UnexpectedEnd, 0);
    EXPECT_PARSE_ERR(String("").Parse<bool>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 0);
    EXPECT_PARSE_ERR(String("-").Parse<int>().UnwrapErr(), ParseErrorCode::UnexpectedEnd, 1);
    EXPECT_PARSE_ERR(String("--123").Parse<int>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 1);
    EXPECT_PARSE_ERR(String("300").Parse<char>().UnwrapErr(), ParseErrorCode::Overflow, 0);
    EXPECT_PARSE_ERR(String("123123123123123").Parse<int>().UnwrapErr(), ParseErrorCode::Overflow, 0);
    EXPECT_PARSE_ERR(String("123qq").Parse<int>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 3);
    EXPECT_PARSE_ERR(String("qq123").Parse<int>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 0);
    EXPECT_PARSE_ERR(String("-123").Parse<unsigned>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 0);
    EXPECT_PARSE_ERR(String("--123").Parse<float>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 0);
    EXPECT_PARSE_ERR(String("123..7").Parse<float>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 4);
    EXPECT_PARSE_ERR(String("qq").Parse<bool>().UnwrapErr(), ParseErrorCode::InvalidSyntax, 0);
}

TEST(Strings, Parse)
{
    EXPECT_EQ(String("123").Parse<Int8>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<Int16>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<Int32>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<Int64>().Unwrap(), 123);

    EXPECT_EQ(String("-123").Parse<Int8>().Unwrap(), -123);
    EXPECT_EQ(String("-123").Parse<Int16>().Unwrap(), -123);
    EXPECT_EQ(String("-123").Parse<Int32>().Unwrap(), -123);
    EXPECT_EQ(String("-123").Parse<Int64>().Unwrap(), -123);

    EXPECT_EQ(String("123").Parse<UInt8>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<UInt16>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<UInt32>().Unwrap(), 123);
    EXPECT_EQ(String("123").Parse<UInt64>().Unwrap(), 123);

    EXPECT_EQ(String("1.5").Parse<Float32>().Unwrap(), 1.5f);
    EXPECT_EQ(String("-1.5").Parse<Float32>().Unwrap(), -1.5f);

    EXPECT_EQ(String("1.5").Parse<Float64>().Unwrap(), 1.5f);
    EXPECT_EQ(String("-1.5").Parse<Float64>().Unwrap(), -1.5f);

    EXPECT_EQ(String("false").Parse<bool>().Unwrap(), false);
    EXPECT_EQ(String("true").Parse<bool>().Unwrap(), true);
    EXPECT_EQ(String("0").Parse<bool>().Unwrap(), false);
    EXPECT_EQ(String("1").Parse<bool>().Unwrap(), true);
}
