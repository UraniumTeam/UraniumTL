#pragma once
#include <UnTL/IO/BaseIO.h>
#include <UnTL/Strings/StringSlice.h>
#include <UnTL/Time/DateTime.h>

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
    } // namespace Internal

    //! \brief Provides common functions to work with directories.
    struct Directory
    {
        //! \brief Get current working directory of the running process.
        [[nodiscard]] static String GetCurrentDirectory();

        //! \brief Get parent of the specified directory without memory allocations.
        //!
        //! \param fileName - The slice of the provided string with only the parent directory.
        [[nodiscard]] static StringSlice GetParent(StringSlice fileName);
    };

    //! \brief Provides common functions to work with files.
    struct File
    {
        //! \brief Check if a file exists.
        //!
        //! \param fileName - The name of the file to check.
        [[nodiscard]] static bool Exists(StringSlice fileName);

        //! \brief Read all an entire text file to a string.
        //!
        //! \param fileName - The name of the file to read.
        //!
        //! \return Either string with the file contents or an error code
        //!
        //! \see ResultCode
        [[nodiscard]] static Result<String, ResultCode> ReadAllText(StringSlice fileName);

        //! \brief Delete a file.
        //!
        //! \param fileName - The name of the file to delete.
        //!
        //! \return An error code if the operation was not successful.
        //!
        //! \see ResultCode
        [[nodiscard]] static VoidResult<ResultCode> Delete(StringSlice fileName);
    };

    //! \brief Represents a file handle.
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

        //! \brief Open a file.
        //!
        //! \param fileName - The path to the file to open.
        //! \param openMode - The OpenMode to use.
        //!
        //! \return An error code if the operation was not successful.
        //!
        //! \see ResultCode
        [[nodiscard]] VoidResult<ResultCode> Open(StringSlice fileName, OpenMode openMode);

        //! \brief Close the handle.
        void Close();

        //! \brief Add offset to the current file position.
        //!
        //! \param offset - Offset to add to the current position.
        //! \param seekMode - The SeekMode to use.
        //!
        //! \return An error code if the operation was not successful.
        //!
        //! \see ResultCode
        [[nodiscard]] VoidResult<ResultCode> Seek(SSize offset, SeekMode seekMode);

        //! \brief Tell current file offset.
        //!
        //! \return Either the current file offset or an error code.
        //!
        //! \see ResultCode
        [[nodiscard]] Result<USize, ResultCode> Tell() const;

        //! \brief Get last modification time of the file.
        //!
        //! \return Either the last modification time or an error code.
        //!
        //! \see ResultCode
        [[nodiscard]] Result<DateTime, ResultCode> GetLastModificationTime() const;

        //! \brief Get current file open mode.
        [[nodiscard]] OpenMode GetOpenMode() const;

        //! \brief Read from the file to a buffer.
        //!
        //! \param buffer - The buffer to read the file data to.
        //! \param size - The size of the provided buffer.
        //!
        //! \return Either the number of bytes actually read or an error code.
        //!
        //! \see ResultCode
        [[nodiscard]] Result<USize, ResultCode> Read(void* buffer, USize size);

        //! \brief Write from buffer to the file.
        //!
        //! \param buffer - The buffer to write the file data from.
        //! \param size   - The size of the provided buffer.
        //!
        //! \return Either the number of bytes actually written or an error code.
        //!
        //! \see ResultCode
        [[nodiscard]] Result<USize, ResultCode> Write(const void* buffer, USize size);

        //! \brief Flush write operations to the file.
        //!
        //! \return An error code if the operation was not successful.
        //!
        //! \see ResultCode
        [[nodiscard]] VoidResult<ResultCode> Flush();

        //! \brief Get length of the file in bytes.
        //!
        //! \return Either the length or an error code.
        //!
        //! \see ResultCode
        [[nodiscard]] Result<USize, ResultCode> Length() const;

        //! \brief Get file name.
        [[nodiscard]] StringSlice GetName() const;

        //! \brief Check if the file is open.
        [[nodiscard]] bool IsOpen() const;
    };
} // namespace UN::IO
