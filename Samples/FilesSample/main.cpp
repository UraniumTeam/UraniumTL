#include <UnTL/IO/FileStream.h>
#include <iostream>

using namespace UN;

inline constexpr StringSlice FileName = "test.txt";
inline constexpr StringSlice Contents = "Test file content";

inline void CreateFileWithStream()
{
    Ptr handle = AllocateObject<IO::FileHandle>();
    Ptr stream = AllocateObject<IO::FileStream>(handle.Get());

    stream->Open(FileName, IO::OpenMode::Create)
        .OrElse([](auto code) {
            std::cout << Fmt::Format("Error while opening file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap();

    stream->WriteFromBuffer(Contents.Data(), Contents.Size())
        .OrElse([](auto code) {
            std::cout << Fmt::Format("Error while writing to file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap();

    std::cout << Fmt::Format("Created a file at: {}", IO::Directory::GetCurrentDirectory() / FileName) << std::endl;
}

inline void ReadFileToString()
{
    auto content = IO::File::ReadAllText(FileName)
                       .OrElse([](auto code) {
                           std::cout << Fmt::Format("Error while reading a file: {}", IO::GetResultDesc(code));
                           return code;
                       })
                       .Unwrap();
    std::cout << Fmt::Format("Read file contents with File::ReadAllText: {}", content) << std::endl;
}

inline void ReadFileWithStream()
{
    Ptr handle = AllocateObject<IO::FileHandle>();
    Ptr stream = AllocateObject<IO::FileStream>(handle.Get());

    stream->Open(FileName, IO::OpenMode::ReadOnly)
        .OrElse([](auto code) {
            std::cout << Fmt::Format("Error while opening file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap();

    String buffer(stream->Length().Expect("Couldn't get length of a stream"), ' ');

    stream->ReadToBuffer(buffer.Data(), buffer.Size())
        .OrElse([](auto code) {
            std::cout << Fmt::Format("Error while reading file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap();

    std::cout << Fmt::Format("Read file contents with a stream: {}", buffer) << std::endl;

    stream->WriteFromBuffer(Contents.Data(), Contents.Size()).OrElse([](auto code) {
        std::cout << Fmt::Format("If you try to write to a read-only stream you get a \"{}\" error", IO::GetResultDesc(code))
                  << std::endl;
        return code;
    });
}

int main()
{
    CreateFileWithStream();
    ReadFileToString();
    ReadFileWithStream();
}
