#pragma once

#include "Interface/FileInputStreamInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuard.h"

#ifndef MENGINE_FILE_STREAM_BUFFER_SIZE
#define MENGINE_FILE_STREAM_BUFFER_SIZE 4096
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidAssetInputStream
        : public FileInputStreamInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AndroidAssetInputStream );

    public:
        AndroidAssetInputStream();
        ~AndroidAssetInputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        void close() override;

    public:
        size_t read( void * const _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        void rewind() override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _size ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    public:
        AAsset * getAsset();

    protected:
        bool openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char *  const _fullPath );
        bool read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read );
        bool seek_( size_t _pos );

    protected:
        AAsset * m_asset;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;
        
        uint8_t m_readCache[MENGINE_FILE_STREAM_BUFFER_SIZE];

        bool m_streaming;
        bool m_share;

        MENGINE_THREAD_GUARD_INIT( AndroidAssetInputStream );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidAssetInputStream, FileInputStreamInterface> AndroidAssetInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
