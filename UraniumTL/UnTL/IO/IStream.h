#pragma once
#include <UnTL/IO/BaseIO.h>
#include <UnTL/Memory/Memory.h>

namespace UN::IO
{
    //! \brief Base interface for I/O streams.
    class IStream : public IObject
    {
    public:
        UN_RTTI_Class(IStream, "FD697DC5-020E-4998-ADF2-9DFAF48E2A75");

        ~IStream() override = default;

        //! \brief True if write operation is allowed on this stream.
        [[nodiscard]] virtual bool WriteAllowed() const noexcept = 0;

        //! \brief True if read operation is allowed on this stream.
        [[nodiscard]] virtual bool ReadAllowed() const noexcept = 0;

        //! \brief True if seek operation is allowed on this stream.
        [[nodiscard]] virtual bool SeekAllowed() const noexcept = 0;

        //! \brief True if stream is open and ready to use.
        [[nodiscard]] virtual bool IsOpen() const = 0;

        //! \brief Add offset to current stream position.
        //!
        //! \param [in] offset   - Offset to add to current position.
        //! \param [in] seekMode - Seek mode to use.
        //!
        //! \return Success or error code.
        [[nodiscard]] virtual VoidResult<ResultCode> Seek(SSize offset, SeekMode seekMode) = 0;

        //! \brief Get current stream position.
        //!
        //! \note This can always return zero for certain streams, e.g. stdio stream.
        //!
        //! \return Current stream position.
        [[nodiscard]] virtual Result<USize, ResultCode> Tell() const = 0;

        //! \brief Get length of the stream.
        //!
        //! This will return length of streams when size is known, e.g. file streams.
        //! Otherwise return value is always zero.
        //!
        //! \return Length of the stream.
        [[nodiscard]] virtual Result<USize, ResultCode> Length() const = 0;

        //! \brief Read contents of stream to a pre-allocated buffer.
        //!
        //! \param [in] buffer - Pointer to buffer to read to.
        //! \param [in] size   - Size in bytes of data to read.
        //!
        //! \return Number of bytes actually read.
        [[nodiscard]] virtual Result<USize, ResultCode> ReadToBuffer(void* buffer, USize size) = 0;

        //! \brief Write contents of buffer to the stream.
        //!
        //! \param [in] buffer - Pointer to buffer to write from.
        //! \param [in] size   - Size in bytes of data to write.
        //!
        //! \return Number of bytes actually written.
        [[nodiscard]] virtual Result<USize, ResultCode> WriteFromBuffer(const void* buffer, USize size) = 0;

        //! \brief Write to this stream from other stream.
        //!
        //! \param [in] stream - Pointer to stream to write from.
        //! \param [in] size   - Size in bytes of data to write.
        //!
        //! \return Number of bytes actually written.
        [[nodiscard]] virtual Result<USize, ResultCode> WriteFromStream(IStream* stream, USize size) = 0;

        //! \brief Get name of the stream.
        //!
        //! Returns file name for file streams and "stdout" for stdout streams. Empty string otherwise.
        //!
        //! \return Name of the stream.
        [[nodiscard]] virtual StringSlice GetName() const = 0;

        //! \brief Get OpenMode for this stream.
        [[nodiscard]] virtual OpenMode GetOpenMode() const = 0;

        //! \brief Close this stream.
        virtual void Close() = 0;
    };
} // namespace UN::IO
