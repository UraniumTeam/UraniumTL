#pragma once
#include <UnTL/IO/IStream.h>

namespace UN::IO
{
    //! \brief Base implementation of IStream interface.
    //!
    //! This class adds default implementation for the WriteFromStream function.
    class StreamBase : public Object<IStream>
    {
    public:
        UN_RTTI_Class(StreamBase, "2F74FF8D-4D81-44BE-962A-9D30669E03C8");

        ~StreamBase() override = default;

        [[nodiscard]] inline Result<USize, ResultCode> WriteFromStream(IStream* stream, USize size) override
        {
            UN_Assert(stream, "Stream was nullptr");
            UN_Assert(stream != this, "Destination and source streams are the same");
            UN_Guard(stream->ReadAllowed(), ResultCode::ReadNotAllowed);
            UN_Guard(WriteAllowed(), ResultCode::WriteNotAllowed);

            static constexpr USize tempSize = 128;
            Int8 temp[tempSize];
            USize result = 0;

            for (USize offset = 0; offset < size; offset += tempSize)
            {
                auto remaining   = size - offset;
                auto currentSize = std::min(remaining, tempSize);

                auto read = stream->ReadToBuffer(temp, currentSize);
                UN_GuardResult(read);
                auto write = WriteFromBuffer(temp, currentSize);
                UN_GuardResult(write);

                result += write.Unwrap();
            }

            return result;
        }
    };

    //! \brief Base implementation for read-only streams.
    class RStreamBase : public StreamBase
    {
    public:
        UN_RTTI_Class(RStreamBase, "339049F5-E6B9-481F-9AC5-4690EDDAF0F5");

        ~RStreamBase() override = default;

        [[nodiscard]] inline bool WriteAllowed() const noexcept override
        {
            return false;
        }

        [[nodiscard]] inline bool ReadAllowed() const noexcept override
        {
            return true;
        }

        [[nodiscard]] inline Result<USize, ResultCode> WriteFromBuffer([[maybe_unused]] const void* buffer,
                                                                       [[maybe_unused]] USize size) override
        {
            return Err(ResultCode::WriteNotAllowed);
        }

        [[nodiscard]] inline Result<USize, ResultCode> WriteFromStream([[maybe_unused]] IStream* stream,
                                                                       [[maybe_unused]] USize size) override
        {
            return Err(ResultCode::WriteNotAllowed);
        }

        [[nodiscard]] inline OpenMode GetOpenMode() const override
        {
            return OpenMode::ReadOnly;
        }
    };

    //! \brief Base implementation for write-only streams.
    class WStreamBase : public StreamBase
    {
    public:
        UN_RTTI_Class(WStreamBase, "41056CA7-2943-4473-8041-3D4DB12619E3");

        [[nodiscard]] inline bool WriteAllowed() const noexcept override
        {
            return true;
        }

        [[nodiscard]] inline bool ReadAllowed() const noexcept override
        {
            return false;
        }

        [[nodiscard]] inline bool SeekAllowed() const noexcept override
        {
            return false;
        }

        [[nodiscard]] inline VoidResult<ResultCode> Seek([[maybe_unused]] SSize offset,
                                                         [[maybe_unused]] SeekMode seekMode) override
        {
            return Err(ResultCode::ReadNotAllowed);
        }

        [[nodiscard]] inline Result<USize, ResultCode> Tell() const override
        {
            return Err(ResultCode::NotSupported);
        }

        [[nodiscard]] inline Result<USize, ResultCode> Length() const override
        {
            return Err(ResultCode::NotSupported);
        }

        [[nodiscard]] inline Result<USize, ResultCode> ReadToBuffer([[maybe_unused]] void* buffer,
                                                                    [[maybe_unused]] USize size) override
        {
            return Err(ResultCode::ReadNotAllowed);
        }

        [[nodiscard]] inline OpenMode GetOpenMode() const override
        {
            return OpenMode::CreateNew;
        }
    };
} // namespace UN::IO
