# UraniumTL
A C++ template library with some alternatives and extensions for STL. Used in other Uranium libraries.

### Example
_Create a file and write some text:_
```cpp
void CreateFileWithStream()
{
    // Smart pointers and reference counting system
    Ptr handle = AllocateObject<IO::FileHandle>();
    Ptr stream = AllocateObject<IO::FileStream>(handle.Get());

    stream->Open(FileName, IO::OpenMode::Create)
        .OrElse([](auto code) {
            // C++20 like Fmt::Format in C++17
            std::cout << Fmt::Format("Error while opening file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap(); // exception-free error handling with Result<T, TError>

    stream->WriteFromBuffer(Contents.Data(), Contents.Size())
        .OrElse([](auto code) {
            std::cout << Fmt::Format("Error while writing to file: {}", IO::GetResultDesc(code)) << std::endl;
            return code;
        })
        .Unwrap();

    // Directory and File classes that implement common functions
    std::cout << Fmt::Format("Created a file at: {}", IO::Directory::GetCurrentDirectory() / FileName) << std::endl;
    
    auto content = IO::File::ReadAllText(FileName)
                       .OrElse([](auto code) {
                           std::cout << Fmt::Format("Error while reading a file: {}", IO::GetResultDesc(code));
                           return code;
                       })
                       .Unwrap();
}
```
