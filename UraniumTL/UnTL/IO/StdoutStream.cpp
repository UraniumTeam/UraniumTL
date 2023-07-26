#include <UnTL/IO/StdoutStream.h>
#include <iostream>

namespace UN::IO
{
    bool StdoutStream::IsOpen() const
    {
        return true;
    }

    Result<USize, ResultCode> StdoutStream::WriteFromBuffer(const void* buffer, USize size)
    {
        UN_Assert(buffer, "Buffer was nullptr");
        std::cout << StringSlice(static_cast<const char*>(buffer), size);
        return 0;
    }

    StringSlice StdoutStream::GetName() const
    {
        return "stdout";
    }

    void StdoutStream::Close() {}
} // namespace UN::IO
