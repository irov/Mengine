#pragma once

#include "Interface/FileInputStreamInterface.h"

#include "Kernel/Factorable.h"

#include "stdex/thread_guard.h"

#include "SDL_rwops.h"

#define MENGINE_FILE_STREAM_BUFFER_SIZE 4096

namespace Mengine
{
    class SDLFileInputStream
        : public FileInputStreamInterface
        , public Factorable
    {
    public:
        SDLFileInputStream();
        ~SDLFileInputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming ) override;

    public:
        size_t read( void * _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t & _time ) const override;

    public:
        bool memory( void ** _memory, size_t * _size ) override;

    protected:
        bool openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _fullPath );
        void close_();
        bool read_( void * _buf, size_t _size, size_t & _read );
        bool seek_( size_t _pos );

    protected:
        SDL_RWops * m_rwops;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;

        uint8_t m_readCache[MENGINE_FILE_STREAM_BUFFER_SIZE] = {0};

        STDEX_THREAD_GUARD_INIT;

#ifdef MENGINE_DEBUG
        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;

        bool m_streaming;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLFileInputStream> SDLFileInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
