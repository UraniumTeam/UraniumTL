#pragma once
#include <UnTL/Buffers/Internal/PoolBucket.h>
#include <UnTL/Containers/ArraySlice.h>
#include <UnTL/Containers/HeapArray.h>
#include <UnTL/Containers/List.h>
#include <UnTL/Utils/BitUtils.h>

namespace UN
{
    //! \brief A pool that allows reusing arrays.
    //!
    //! The class enables renting and returning arrays frequently while reducing
    //! the number of memory allocations by caching arrays with different sizes.
    //!
    //! \note The class is thread-safe.
    //!
    //! \tparam T - Type of array element, must be trivially destructible.
    template<class T, class TLock = std::mutex>
    class ArrayPool final : public Object<IObject>
    {
        using PoolBucket = Internal::PoolBucket<T, TLock>;

        inline static constexpr USize DefaultMaxArrayLength         = 1024 * 1024;
        inline static constexpr USize DefaultMaxArrayCountPerBucket = 50;

        static_assert(std::is_trivially_destructible_v<T> && !std::is_const_v<T>);

        ArraySlice<PoolBucket> m_Buckets;
        IAllocator* m_pAllocator;

        template<class TItem>
        UN_FINLINE ArraySlice<TItem> AllocateStorage(USize length)
        {
            void* pData = m_pAllocator->Allocate(length * sizeof(TItem), alignof(TItem));
            return ArraySlice<TItem>(static_cast<TItem*>(pData), length);
        }

        template<class TItem>
        UN_FINLINE void DeallocateStorage(const ArraySlice<TItem>& storage)
        {
            if (storage.Empty())
            {
                return;
            }

            m_pAllocator->Deallocate(storage.Data());
        }

        inline UInt32 Log2(UInt32 x)
        {
            return 31 ^ Bits::CountLeadingZeros(x | 1);
        }

        inline USize SelectBucketIndex(USize bufferSize)
        {
            return Log2(static_cast<UInt32>(bufferSize) - 1 | 15) - 3;
        }

        inline USize GetMaxSizeForBucket(int binIndex)
        {
            return 16 << binIndex;
        }

    public:
        //! \brief Create a default instance of ArrayPool<T>.
        inline explicit ArrayPool(IAllocator* pAllocator)
            : ArrayPool(pAllocator, DefaultMaxArrayLength, DefaultMaxArrayCountPerBucket)
        {
        }

        //! \brief Create a default instance of ArrayPool<T>.
        inline explicit ArrayPool(IAllocator* pAllocator, USize maxArrayLength, USize maxArrayCountPerBucket)
            : m_pAllocator(pAllocator)
        {
            const auto MinimumArrayLength = 0x10, MaximumArrayLength = 0x40000000;
            if (maxArrayLength > MaximumArrayLength)
            {
                maxArrayLength = MaximumArrayLength;
            }
            else if (maxArrayLength < MinimumArrayLength)
            {
                maxArrayLength = MinimumArrayLength;
            }

            auto maxBuckets = SelectBucketIndex(maxArrayLength);
            m_Buckets       = AllocateStorage<PoolBucket>(maxBuckets + 1);
            for (USize i = 0; i < maxBuckets + 1; ++i)
            {
                new (&m_Buckets[i]) PoolBucket(pAllocator, GetMaxSizeForBucket(i), maxArrayCountPerBucket);
            }
        }

        ~ArrayPool() override
        {
            std::destroy(m_Buckets.begin(), m_Buckets.end());
            DeallocateStorage(m_Buckets);
            m_Buckets = {};
        }

        //! \brief Retrieve an array with specified length.
        //!
        //! This function will find an array with length equal to the smallest power
        //! of two that is greater that the length specified. However it returns only
        //! a slice of that array. The length of the returned slice is the same as
        //! specified in the length argument.
        //!
        //! \note The pool doesn't clear previously rented arrays, so this function can
        //! return an array with garbage data.
        //!
        //! \param length - The length of the array to retrieve.
        //!
        //! \return The rented array.
        [[nodiscard]] inline ArraySlice<T> Rent(USize length) noexcept
        {
            UN_Assert(length >= 0, "Length must be non-negative");
            if (length == 0)
            {
                return {};
            }

            ArraySlice<T> buffer;
            auto index = SelectBucketIndex(length);
            if (index < m_Buckets.Length())
            {
                const auto MaxBucketsToTry = 2;

                auto i = index;
                do
                {
                    buffer = m_Buckets[i].Rent();
                    if (buffer.Any())
                    {
                        return buffer;
                    }
                }
                while (++i < m_Buckets.Length() && i != index + MaxBucketsToTry);

                return m_Buckets[index].AllocateStorage();
            }

            return AllocateStorage<T>(length);
        }

        //! \brief Return a previously rented array.
        //!
        //! The returned array must be obtained via the Rent function called from the
        //! same ArrayPool instance. Once returned the array must never be used, because
        //! it can be rented by one of the subsequent calls to the Rent function or the
        //! memory of this array can be freed completely.
        //!
        //! \param array - The array previously rented from the pool to return.
        inline void Return(const ArraySlice<T>& array) noexcept
        {
            if (array.Empty())
            {
                return;
            }

            auto bucket     = SelectBucketIndex(array.Length());
            auto haveBucket = bucket < m_Buckets.Length();

            if (haveBucket)
            {
                m_Buckets[bucket].Return(array);
            }
            else
            {
                DeallocateStorage(array);
            }
        }
    };
} // namespace UN
