#include <UnTL/RTTI/RTTI.h>
#include <gtest/gtest.h>

using namespace UN;

class Base
{
public:
    UN_RTTI_Class(Base, "0AC35B98-FDDF-49BC-969D-943CEA5F96D7");
};

class Bar : public Base
{
public:
    UN_RTTI_Class(Bar, "C0C21CE7-67B6-4EB2-BF77-3B44C5E9B814");
};

class Foo : public Base
{
public:
    UN_RTTI_Class(Foo, "24BA29F3-C85A-46DF-A51F-A18FD14B1606");
};

TEST(RTTI, TypeName)
{
    Foo foo;
    auto expectedName = "class Foo";
    ASSERT_EQ(foo.UnRTTI_GetName(), expectedName);
    ASSERT_EQ(Foo::UnRTTI_GetSName(), expectedName);
}

TEST(RTTI, DerivedTypeID)
{
    Foo foo;
    Base* base = &foo;
    ASSERT_EQ(base->UnRTTI_GetID(), foo.UnRTTI_GetID());
}

TEST(RTTI, DynamicCast)
{
    Bar bar;
    Base* base = &bar;
    ASSERT_EQ(&bar, un_dynamic_cast<Bar*>(base));
    ASSERT_EQ(nullptr, un_dynamic_cast<Foo*>(base));
}
