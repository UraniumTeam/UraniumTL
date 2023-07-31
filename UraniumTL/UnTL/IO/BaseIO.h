#pragma once
#include <UnTL/Strings/StringSlice.h>
#include <UnTL/Utils/Result.h>

namespace UN::IO
{
    //! \brief Result code of an I/O operation.
    enum class ResultCode
    {
        Success, //!< Operation succeeded.

        PermissionDenied,  //!< Permission denied.
        NoFileOrDirectory, //!< No such file or directory.
        FileExists,        //!< File already exists.
        FileTooLarge,      //!< File is too large.
        FilenameTooLong,   //!< Filename is too long.
        NotDirectory,      //!< Not a directory.
        IsDirectory,       //!< Is a directory.
        DirectoryNotEmpty, //!< Directory is not empty.
        TooManyOpenFiles,  //!< Too many files are open.
        InvalidSeek,       //!< Invalid seek operation.
        IOError,           //!< IO error.
        DeadLock,          //!< Resource deadlock would occur.
        UnknownError,      //!< Unknown error.

        ReadNotAllowed,  //!< Read operation is not allowed.
        WriteNotAllowed, //!< Write operation is not allowed.
        NotSupported,    //!< Operation is not supported.
        NotOpen          //!< File ot stream is not open.
    };

    //! \brief Get result code description.
    StringSlice GetResultDesc(ResultCode code);

    //! \brief File or stream open mode.
    enum class OpenMode
    {
        None,      //!< None mode.
        ReadOnly,  //!< Read only open mode.
        WriteOnly, //!< Write only open mode.
        Append,    //!< Append open mode.
        Create,    //!< Create (new or rewrite existing) open mode.
        CreateNew, //!< Create (new only) open mode.
        Truncate,  //!< Truncate open mode.
        ReadWrite  //!< Read and write open mode.
    };

    //! \brief Check if writing is allowed with the specified open mode.
    //!
    //! \param mode - The open mode to check.
    inline bool IsWriteAllowed(OpenMode mode)
    {
        switch (mode)
        {
        case OpenMode::WriteOnly:
        case OpenMode::Append:
        case OpenMode::Create:
        case OpenMode::CreateNew:
        case OpenMode::Truncate:
        case OpenMode::ReadWrite:
            return true;
        default:
            return false;
        }
    }

    //! \brief Check if reading is allowed with the specified open mode.
    //!
    //! \param mode - The open mode to check.
    inline bool IsReadAllowed(OpenMode mode)
    {
        switch (mode)
        {
        case OpenMode::ReadOnly:
        case OpenMode::ReadWrite:
            return true;
        default:
            return false;
        }
    }

    //! \brief Seek mode.
    enum class SeekMode
    {
        Begin,  //!< Begin seek mode.
        End,    //!< End seek mode.
        Current //!< Current seek mode.
    };
} // namespace UN::IO
