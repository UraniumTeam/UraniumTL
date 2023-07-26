#pragma once
#include <UnTL/IO/StreamBase.h>
#include <UnTL/IO/FileHandle.h>

namespace UN::IO
{
    class FileStream : public StreamBase
    {
        Ptr<FileHandle> m_Handle;

    public:
        explicit FileStream(FileHandle* file);

        ~FileStream() override = default;

        [[nodiscard]] VoidResult<ResultCode> Open(StringSlice fileName, OpenMode openMode);

        [[nodiscard]] bool WriteAllowed() const noexcept override;
        [[nodiscard]] bool ReadAllowed() const noexcept override;
        [[nodiscard]] bool SeekAllowed() const noexcept override;
        [[nodiscard]] bool IsOpen() const override;
        [[nodiscard]] VoidResult<ResultCode> Seek(SSize offset, SeekMode seekMode) override;
        [[nodiscard]] Result<USize, ResultCode> Tell() const override;
        [[nodiscard]] Result<USize, ResultCode> Length() const override;
        [[nodiscard]] Result<USize, ResultCode> ReadToBuffer(void* buffer, USize size) override;
        [[nodiscard]] Result<USize, ResultCode> WriteFromBuffer(const void* buffer, USize size) override;
        [[nodiscard]] StringSlice GetName() const override;
        [[nodiscard]] OpenMode GetOpenMode() const override;
        void Close() override;
    };
}
