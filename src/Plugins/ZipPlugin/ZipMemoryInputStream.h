#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class ZipMemoryInputStream
        : public MemoryInputInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( ZipMemoryInputStream );

    public:
        ZipMemoryInputStream();
        ~ZipMemoryInputStream() override;

    public:
        void setMemoryInputStream( const MemoryInputInterfacePtr & _stream );
        const MemoryInputInterfacePtr & getMemoryInputStream() const;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath );
        bool close();

    public:
        Pointer newBuffer( size_t _size ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;

    public:
        size_t read( void * const _buffer, size_t _size ) override;
        bool seek( size_t _carriage ) override;
        void rewind() override;
        bool rseek( size_t _carriage ) override;
        bool skip( size_t _offset ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        MemoryInputInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ZipMemoryInputStream, InputStreamInterface> ZipMemoryInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}