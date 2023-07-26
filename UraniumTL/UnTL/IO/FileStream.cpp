#include <UnTL/IO/FileStream.h>

namespace UN::IO
{
    FileStream::FileStream(FileHandle* file)
        : m_Handle(file)
    {
    }

    bool FileStream::WriteAllowed() const noexcept
    {
        return IsWriteAllowed(GetOpenMode());
    }

    bool FileStream::ReadAllowed() const noexcept
    {
        return IsReadAllowed(GetOpenMode());
    }

    bool FileStream::SeekAllowed() const noexcept
    {
        return true;
    }

    bool FileStream::IsOpen() const
    {
        return m_Handle->IsOpen();
    }

    VoidResult<ResultCode> FileStream::Seek(SSize offset, SeekMode seekMode)
    {
        return m_Handle->Seek(offset, seekMode);
    }

    Result<USize, ResultCode> FileStream::Tell() const
    {
        return m_Handle->Tell();
    }

    Result<USize, ResultCode> FileStream::Length() const
    {
        return m_Handle->Length();
    }

    Result<USize, ResultCode> FileStream::ReadToBuffer(void* buffer, USize size)
    {
        return m_Handle->Read(buffer, size);
    }

    Result<USize, ResultCode> FileStream::WriteFromBuffer(const void* buffer, USize size)
    {
        return m_Handle->Write(buffer, size);
    }

    StringSlice FileStream::GetName() const
    {
        return m_Handle->GetName();
    }

    OpenMode FileStream::GetOpenMode() const
    {
        return m_Handle->GetOpenMode();
    }

    void FileStream::Close()
    {
        m_Handle->Close();
    }

    VoidResult<ResultCode> FileStream::Open(StringSlice fileName, OpenMode openMode)
    {
        return m_Handle->Open(fileName, openMode);
    }
} // namespace UN::IO
