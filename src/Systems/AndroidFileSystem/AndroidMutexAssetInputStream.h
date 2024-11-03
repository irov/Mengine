#pragma once

#include "Interface/FileInputStreamInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "AndroidAssetInputStream.h"

#include "Kernel/Factorable.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/BaseDebugFile.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidMutexAssetInputStream
        : public FileInputStreamInterface
        , public Factorable
#if defined(MENGINE_DEBUG)
        , public BaseDebugFile
#endif
    {
        DECLARE_FACTORABLE( AndroidMutexAssetInputStream );

    public:
        AndroidMutexAssetInputStream();
        ~AndroidMutexAssetInputStream() override;

    public:
        void setAssetInputStream( const AndroidAssetInputStreamPtr & _stream );
        void setThreadMutex( const ThreadMutexInterfacePtr & _mutex );

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        bool close() override;

    public:
        size_t read( void * const _buf, size_t _size ) override;
        bool seek( size_t _pos ) override;
        void rewind() override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _size ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t *  const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        bool read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read );
        bool seek_( size_t _pos );

    protected:
        AndroidAssetInputStreamPtr m_stream;
        ThreadMutexInterfacePtr m_mutex;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;        

        uint8_t m_readCache[MENGINE_FILE_STREAM_BUFFER_SIZE];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidMutexAssetInputStream, FileInputStreamInterface> AndroidMutexAssetInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
