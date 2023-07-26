#pragma once
#include <UnTL/IO/StreamBase.h>

namespace UN::IO
{
    class StdoutStream : public WStreamBase
    {
    public:
        UN_RTTI_Class(StdoutStream, "2D5441F8-10B1-4358-B486-5C6BF02DDB24");

        [[nodiscard]] bool IsOpen() const override;
        [[nodiscard]] Result<USize, ResultCode> WriteFromBuffer(const void* buffer, USize size) override;
        [[nodiscard]] StringSlice GetName() const override;
        void Close() override;
    };
} // namespace UN::IO
