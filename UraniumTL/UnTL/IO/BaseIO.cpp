#include <UnTL/IO/BaseIO.h>

namespace UN::IO
{
    StringSlice GetResultDesc(ResultCode code)
    {
        switch (code)
        {
        case ResultCode::Success:
            return "Operation succeeded";
        case ResultCode::PermissionDenied:
            return "Permission denied";
        case ResultCode::NoFileOrDirectory:
            return "No such file or directory";
        case ResultCode::FileExists:
            return "File already exists";
        case ResultCode::FileTooLarge:
            return "File is too large";
        case ResultCode::FilenameTooLong:
            return "Filename is too long";
        case ResultCode::NotDirectory:
            return "Not a directory";
        case ResultCode::IsDirectory:
            return "Is a directory";
        case ResultCode::DirectoryNotEmpty:
            return "Directory is not empty";
        case ResultCode::TooManyOpenFiles:
            return "Too many files are open";
        case ResultCode::InvalidSeek:
            return "Invalid seek operation";
        case ResultCode::IOError:
            return "IO error";
        case ResultCode::DeadLock:
            return "Resource deadlock would occur";
        case ResultCode::ReadNotAllowed:
            return "Read operation is not allowed";
        case ResultCode::WriteNotAllowed:
            return "Write operation is not allowed";
        case ResultCode::NotSupported:
            return "Operation is not supported";
        case ResultCode::NotOpen:
            return "File ot stream is not open";
        default:
            return "Unknown error";
        }
    }
} // namespace UN::IO
