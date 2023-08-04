#pragma once
#include <UnTL/Memory/IAllocator.h>
#include <UnTL/RTTI/RTTI.h>
#include <atomic>

namespace UN
{
    //! \brief This allocator uses platform-specific aligned versions of malloc() and free().
    class SystemAllocator : public IAllocator
    {
        static SystemAllocator m_Instance;

        std::atomic<Int64> m_Count;

    public:
        UN_RTTI_Class(SystemAllocator, "6C2B53D6-3A9A-447F-A127-1052253189C4");

        void* Allocate(USize size, USize alignment) override;
        void Deallocate(void* pointer) override;
        [[nodiscard]] const char* GetName() const override;

        [[nodiscard]] Int64 AllocationCount() const;

        //! \brief Get global static instance of the system allocator.
        inline static SystemAllocator* Get()
        {
            return &m_Instance;
        }
    };

    inline SystemAllocator SystemAllocator::m_Instance;

    inline void* SystemAllocator::Allocate(USize size, USize alignment)
    {
        m_Count.fetch_add(1, std::memory_order_relaxed);
        return UN_AlignedMalloc(size, alignment);
    }

    inline void SystemAllocator::Deallocate(void* pointer)
    {
        m_Count.fetch_sub(1, std::memory_order_relaxed);
        return UN_AlignedFree(pointer);
    }

    inline const char* SystemAllocator::GetName() const
    {
        return "System allocator";
    }

    inline Int64 SystemAllocator::AllocationCount() const
    {
        return m_Count.load(std::memory_order_acquire);
    }
} // namespace UN
