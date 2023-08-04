#include <Tests/Common/Common.h>
#include <UnTL/Buffers/ArrayPool.h>

using namespace UN;

TEST(ArrayPool, RentReturn)
{
    auto allocatedBefore = SystemAllocator::Get()->AllocationCount();
    {
        Ptr pool = AllocateObject<ArrayPool<int>>(SystemAllocator::Get());

        auto arr = pool->Rent(1000);
        EXPECT_GE(arr.Length(), 1000);

        auto* lastAddress = arr.Data();

        pool->Return(arr);
        arr = pool->Rent(1000);
        EXPECT_EQ(arr.Data(), lastAddress);
        pool->Return(arr);
    }
    EXPECT_EQ(allocatedBefore, SystemAllocator::Get()->AllocationCount());
}

TEST(ArrayPool, RentMultiple)
{
    auto allocatedBefore = SystemAllocator::Get()->AllocationCount();
    {
        Ptr pool = AllocateObject<ArrayPool<int>>(SystemAllocator::Get());

        for (Int32 size = 10; size < 10000; size *= 10)
        {
            List<ArraySlice<int>> arrays;
            for (Int32 i = 0; i < 1000; ++i)
            {
                arrays.Push(pool->Rent(size));
            }

            arrays.Sort([](auto& l, auto& r) {
                return l.Data() < r.Data();
            });

            for (Int32 i = 1; i < arrays.Size(); ++i)
            {
                ASSERT_NE(arrays[i - 1].Data(), arrays[i].Data());
            }

            for (Int32 i = 0; i < 1000; ++i)
            {
                ASSERT_GE(arrays[i].Length(), size);
                pool->Return(arrays[i]);
            }
        }
    }
    EXPECT_EQ(allocatedBefore, SystemAllocator::Get()->AllocationCount());
}
