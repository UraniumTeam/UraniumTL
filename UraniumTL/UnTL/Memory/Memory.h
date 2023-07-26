#pragma once
#include <UnTL/Memory/Ptr.h>
#include <UnTL/Memory/SystemAllocator.h>
#include <UnTL/RTTI/RTTI.h>
#include <unordered_map>

namespace UN
{
    //! \brief Create a reference counted object.
    //!
    //! This function allocates storage for a ReferenceCounter and an object of type T.
    //! It attaches reference counter to the allocated object.
    //!
    //! \param pAllocator - Allocator to use for allocation and deallocation of the object.
    //! \param args       - Arguments to call constructor of T with.
    //!
    //! \tparam T          - Type of object to allocate.
    //! \tparam TAllocator - Type of allocator to use for allocation and deallocation of the object.
    //! \tparam Args       - Types of arguments to call constructor of T with.
    //!
    //! \return The created object.
    template<class T, class TAllocator, class... Args>
    inline T* AllocateObjectEx(TAllocator* pAllocator, Args&&... args)
    {
        USize counterSize = AlignUp(sizeof(ReferenceCounter), alignof(T));
        USize wholeSize   = sizeof(T) + counterSize;

        auto* ptr     = static_cast<UInt8*>(pAllocator->Allocate(wholeSize, alignof(T)));
        auto* counter = new (ptr) ReferenceCounter(pAllocator);

        T* object = new (ptr + counterSize) T(std::forward<Args>(args)...);
        object->AttachRefCounter(counter);
        return object;
    }

    //! \brief Create a reference counted object.
    //!
    //! This function allocates storage for a ReferenceCounter and an object of type T.
    //! It attaches reference counter to the allocated object.
    //!
    //! \param args - Arguments to call constructor of T with.
    //!
    //! \tparam T    - Type of object to allocate.
    //! \tparam Args - Types of arguments to call constructor of T with.
    //!
    //! \return The created object.
    template<class T, class... Args>
    inline T* AllocateObject(Args&&... args)
    {
        return AllocateObjectEx<T, SystemAllocator>(SystemAllocator::Get(), std::forward<Args>(args)...);
    }

    //! \brief A wrapper for IAllocator compatible with `std::allocator`.
    template<class T, class TAlloc>
    class StdAllocator final
    {
        mutable IAllocator* m_Instance;

    public:
        UN_RTTI_Struct(StdAllocator, "18778CFF-60EF-49FA-A560-46CE59C2BEFF");

        using value_type = T;

        inline StdAllocator(const StdAllocator& other) noexcept
            : m_Instance(other.m_Instance)
        {
        }

        template<class TOther>
        inline explicit StdAllocator(const StdAllocator<TOther, TAlloc>& other) noexcept
            : m_Instance(other.GetImpl())
        {
        }

        inline StdAllocator() noexcept
        {
            m_Instance = SystemAllocator::Get();
        }

        inline explicit StdAllocator(TAlloc* instance) noexcept
        {
            m_Instance = instance;
        }

        inline IAllocator* GetImpl() const noexcept
        {
            return m_Instance;
        }

        inline value_type* allocate(size_t n) noexcept
        {
            return static_cast<value_type*>(m_Instance->Allocate(n * sizeof(T), alignof(T)));
        }

        inline void deallocate(value_type* ptr, size_t) noexcept
        {
            m_Instance->Deallocate(ptr);
        }
    };

    template<class T1, class T2, class TAlloc>
    inline bool operator==(const StdAllocator<T1, TAlloc>&, const StdAllocator<T2, TAlloc>&) noexcept
    {
        return true;
    }

    template<class T1, class T2, class TAlloc>
    inline bool operator!=(const StdAllocator<T1, TAlloc>& x, const StdAllocator<T2, TAlloc>& y) noexcept
    {
        return !(x == y);
    }

    //! \brief An alias for StdAllocator that uses HeapAllocator.
    template<class T>
    using StdHeapAllocator = StdAllocator<T, SystemAllocator>;

    //! \brief An alias for `std::wstring` that uses HeapAllocator through StdHeapAllocator.
    using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>, StdHeapAllocator<wchar_t>>;

    //! \brief An alias for `std::unordered_map` that uses HeapAllocator through StdHeapAllocator.
    template<class TKey, class TValue, class THasher = std::hash<TKey>, class TEq = std::equal_to<TKey>>
    using UnorderedMap = std::unordered_map<TKey, TValue, THasher, TEq, StdHeapAllocator<std::pair<const TKey, TValue>>>;
} // namespace UN
