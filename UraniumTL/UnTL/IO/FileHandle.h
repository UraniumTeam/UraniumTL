#pragma once
#include <UnTL/Strings/StringSlice.h>
#include <UnTL/Time/DateTime.h>
#include <UnTL/IO/BaseIO.h>

namespace UN::IO
{
    namespace Internal
    {
        inline ResultCode GetResultCode(errno_t err)
        {
            switch (err)
            {
            case ENOENT:
                return ResultCode::NoFileOrDirectory;
            case EACCES:
                return ResultCode::PermissionDenied;
            case ENAMETOOLONG:
                return ResultCode::FilenameTooLong;
            case EIO:
                return ResultCode::IOError;
            case EEXIST:
                return ResultCode::FileExists;
            case ENOTDIR:
                return ResultCode::NotDirectory;
            case EISDIR:
                return ResultCode::IsDirectory;
            case EMFILE:
                return ResultCode::TooManyOpenFiles;
            case EFBIG:
                return ResultCode::FileTooLarge;
            case ENOTEMPTY:
                return ResultCode::DirectoryNotEmpty;
            case EDEADLK:
                return ResultCode::DeadLock;
            case ESPIPE:
                return ResultCode::InvalidSeek;
            default:
                return ResultCode::UnknownError;
            }
        }
    }

    struct Directory
    {
        [[nodiscard]] static String GetCurrentDirectory();
        [[nodiscard]] static StringSlice GetParent(StringSlice fileName);
    };

    struct File
    {
        [[nodiscard]] static bool Exists(StringSlice fileName);
        [[nodiscard]] static Result<String, ResultCode> ReadAllText(StringSlice fileName);
        [[nodiscard]] static VoidResult<ResultCode> Delete(StringSlice fileName);
    };

    class FileHandle final : public Object<IObject>
    {
        FILE* m_Handle;
        String m_FileName{};

        char m_OpenModeString[8]{};
        OpenMode m_OpenMode = OpenMode::None;

        void GenFileOpenMode(OpenMode openMode);

    public:
        UN_RTTI_Class(FileHandle, "58D19D75-CE53-4B11-B151-F82583B3EAD8");

        FileHandle();
        ~FileHandle() override;

        [[nodiscard]] VoidResult<ResultCode> Open(StringSlice fileName, OpenMode openMode);
        void Close();

        [[nodiscard]] VoidResult<ResultCode> Seek(SSize offset, SeekMode seekMode);

        [[nodiscard]] Result<USize, ResultCode> Tell() const;

        [[nodiscard]] Result<DateTime, ResultCode> GetLastModificationTime() const;
        [[nodiscard]] OpenMode GetOpenMode() const;

        [[nodiscard]] Result<USize, ResultCode> Read(void* buffer, USize size);
        [[nodiscard]] Result<USize, ResultCode> Write(const void* buffer, USize size);

        [[nodiscard]] VoidResult<ResultCode> Flush();

        [[nodiscard]] Result<USize, ResultCode> Length() const;

        [[nodiscard]] StringSlice GetName() const;

        [[nodiscard]] bool IsOpen() const;
    };
} // namespace UN::IO
