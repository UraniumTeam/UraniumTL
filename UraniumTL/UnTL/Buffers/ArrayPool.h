#pragma once
#include <UnTL/Containers/ArraySlice.h>
#include <UnTL/Containers/List.h>

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
    template<class T>
    class ArrayPool final
    {
        inline static constexpr USize Alignment = std::max(static_cast<USize>(16), alignof(T));

        static_assert(std::is_trivially_destructible_v<T> && !std::is_const_v<T>);

        IAllocator* m_pAllocator;

        inline explicit ArrayPool(IAllocator* pAllocator)
            : m_pAllocator(pAllocator)
        {
        }

        // TODO: Implement actual array pool.
        //  The current implementation is very basic, it only allocates and deallocates
        //  the arrays without caching.

        UN_FINLINE ArraySlice<T> AllocateStorage(USize count)
        {
            void* pData = m_pAllocator->Allocate(count * sizeof(T), Alignment);
            return ArraySlice<T>(static_cast<T*>(pData), count);
        }

        UN_FINLINE void DeallocateStorage(const ArraySlice<T>& storage)
        {
            if (storage.Empty())
            {
                return;
            }

            m_pAllocator->Deallocate(storage.Data());
        }

    public:
        //! \brief Create a default instance of ArrayPool<T>.
        //!
        //! \return The created instance.
        [[nodiscard]] inline static ArrayPool Create() noexcept
        {
            return ArrayPool(SystemAllocator::Get());
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
        //! \return The rented array.
        [[nodiscard]] inline ArraySlice<T> Rent(USize length) noexcept
        {
            return AllocateStorage(length);
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
            DeallocateStorage(array);
        }
    };
} // namespace UN
