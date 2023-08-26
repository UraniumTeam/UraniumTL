#include <UnTL/IO/FileHandle.h>

#if UN_WINDOWS
#    include <direct.h>
#    define UN_SEEK_64 _fseeki64
#    define UN_TELL_64 _ftelli64
#else
#    include <sys/stat.h>
#    include <unistd.h>
#    define UN_SEEK_64 fseek
#    define UN_TELL_64 ftell
#endif

namespace UN::IO
{
    FileHandle::FileHandle()
        : m_Handle(nullptr)
    {
    }

    FileHandle::~FileHandle()
    {
        Close();
    }

    VoidResult<ResultCode> FileHandle::Open(StringSlice fileName, OpenMode openMode)
    {
        switch (openMode)
        {
        case OpenMode::ReadOnly:
        case OpenMode::WriteOnly:
        case OpenMode::Append:
        case OpenMode::ReadWrite:
            UN_Guard(File::Exists(fileName), ResultCode::NoFileOrDirectory);
            break;
        case OpenMode::CreateNew:
            UN_Guard(!File::Exists(fileName), ResultCode::FileExists);
            break;
        default:
            break;
        }

        Close();
        GenFileOpenMode(openMode);
        m_Handle = fopen(fileName.Data(), m_OpenModeString);
        if (m_Handle)
        {
            m_FileName = fileName;
            m_OpenMode = openMode;
            return OK();
        }

        return Err(Internal::GetResultCode(errno));
    }

    void FileHandle::Close()
    {
        if (IsOpen())
        {
            fclose(m_Handle);
            m_Handle   = nullptr;
            m_OpenMode = OpenMode::None;
            m_FileName.Clear();
        }
    }

    void FileHandle::GenFileOpenMode(OpenMode openMode)
    {
        SSize index = 0;
        switch (openMode)
        {
        case OpenMode::ReadOnly:
            m_OpenModeString[index++] = 'r';
            break;
        case OpenMode::Append:
            m_OpenModeString[index++] = 'a';
            break;
        case OpenMode::WriteOnly:
        case OpenMode::Create:
        case OpenMode::CreateNew:
            m_OpenModeString[index++] = 'w';
            break;
        case OpenMode::ReadWrite:
            m_OpenModeString[index++] = 'r';
            m_OpenModeString[index++] = '+';
            break;
        case OpenMode::Truncate:
            m_OpenModeString[index++] = 'w';
            m_OpenModeString[index++] = '+';
            break;
        default:
            UN_Unreachable("Invalid FileOpenMode");
        }

        m_OpenModeString[index] = 'b';
    }

    VoidResult<ResultCode> FileHandle::Seek(SSize offset, SeekMode seekMode)
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);

        int origin = 0;
        switch (seekMode)
        {
        case SeekMode::Begin:
            origin = SEEK_SET;
            break;
        case SeekMode::End:
            origin = SEEK_END;
            break;
        case SeekMode::Current:
            origin = SEEK_CUR;
            break;
        }

        if (UN_SEEK_64(m_Handle, offset, origin))
        {
            return Err(ResultCode::InvalidSeek);
        }

        return OK();
    }

    Result<USize, ResultCode> FileHandle::Tell() const
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        return UN_TELL_64(m_Handle);
    }

    Result<USize, ResultCode> FileHandle::Read(void* buffer, USize size)
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        auto result = fread(buffer, 1, size, m_Handle);
        if (result == 0 && ferror(m_Handle))
        {
            return Err(Internal::GetResultCode(errno));
        }

        return result;
    }

    Result<USize, ResultCode> FileHandle::Write(const void* buffer, USize size)
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        UN_Guard(IsWriteAllowed(GetOpenMode()), ResultCode::WriteNotAllowed);
        auto result = fwrite(buffer, 1, size, m_Handle);
        if (result == 0 && ferror(m_Handle))
        {
            return Err(Internal::GetResultCode(errno));
        }

        return result;
    }

    VoidResult<ResultCode> FileHandle::Flush()
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        fflush(m_Handle);
        return OK();
    }

    Result<USize, ResultCode> FileHandle::Length() const
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        struct stat st; // NOLINT
        if (stat(m_FileName.Data(), &st) == 0)
        {
            return st.st_size;
        }

        return Err(Internal::GetResultCode(errno));
    }

    Result<DateTime, ResultCode> FileHandle::GetLastModificationTime() const
    {
        UN_Guard(IsOpen(), ResultCode::NotOpen);
        struct stat st; // NOLINT
        if (stat(m_FileName.Data(), &st) == 0)
        {
            return DateTime::CreateLocal(st.st_mtime);
        }

        return Err(Internal::GetResultCode(errno));
    }

    StringSlice FileHandle::GetName() const
    {
        return m_FileName;
    }

    bool FileHandle::IsOpen() const
    {
        return m_Handle;
    }

    OpenMode FileHandle::GetOpenMode() const
    {
        return m_OpenMode;
    }

    String Directory::GetCurrentDirectory()
    {
        static char buffer[256];
#if UN_WINDOWS
        _getcwd(buffer, sizeof(buffer));
#else
        getcwd(buffer, sizeof(buffer));
#endif
        return buffer;
    }

    StringSlice Directory::GetParent(StringSlice fileName)
    {
        auto endIter = fileName.FindLastOf(UN_PATH_SEPARATOR);
        if (endIter == fileName.end())
        {
            endIter = fileName.FindLastOf('/');
        }

        return { fileName.begin(), endIter };
    }

    bool File::Exists(StringSlice fileName)
    {
        struct stat buffer; // NOLINT
        return stat(fileName.Data(), &buffer) == 0;
    }

    VoidResult<ResultCode> File::Delete(StringSlice fileName)
    {
        if (remove(fileName.Data()) == 0)
        {
            return OK();
        }

        return Err(Internal::GetResultCode(errno));
    }

    Result<String, ResultCode> File::ReadAllText(StringSlice fileName)
    {
        FileHandle file;
        auto open = file.Open(fileName, OpenMode::ReadOnly);
        UN_GuardResult(open);

        auto lengthRes = file.Length();
        UN_Guard(lengthRes, lengthRes.UnwrapErr());

        auto length = lengthRes.Unwrap();
        String buffer(length, ' ');
        auto read = file.Read(buffer.Data(), length);
        UN_GuardResult(read);
        return buffer;
    }
} // namespace UN::IO
