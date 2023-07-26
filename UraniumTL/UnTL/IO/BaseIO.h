#pragma once
#include <UnTL/Strings/StringSlice.h>
#include <UnTL/Utils/Result.h>

namespace UN::IO
{
    enum class ResultCode
    {
        Success,

        PermissionDenied,
        NoFileOrDirectory,
        FileExists,
        FileTooLarge,
        FilenameTooLong,
        NotDirectory,
        IsDirectory,
        DirectoryNotEmpty,
        TooManyOpenFiles,
        InvalidSeek,
        IOError,
        DeadLock,
        UnknownError,

        ReadNotAllowed,
        WriteNotAllowed,
        NotSupported,
        NotOpen
    };

    StringSlice GetResultDesc(ResultCode code);

    enum class OpenMode
    {
        None,
        ReadOnly,
        WriteOnly,
        Append,
        Create,
        CreateNew,
        Truncate,
        ReadWrite
    };

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

    enum class SeekMode
    {
        Begin,
        End,
        Current
    };
} // namespace UN::IO
