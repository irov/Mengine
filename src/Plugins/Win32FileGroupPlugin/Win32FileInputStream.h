#pragma once

#include "Interface/FileInputStreamInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ThreadGuardScope.h"

#include "Environment/Windows/WindowsIncluder.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE
#define MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE 4096
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class Win32FileInputStream
        : public FileInputStreamInterface
        , public Factorable
    {
    public:
        Win32FileInputStream();
        ~Win32FileInputStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        bool close() override;

    public:
        size_t read( void * const _buf, size_t _count ) override;
        bool seek( size_t _pos ) override;
        bool rseek( size_t _pos ) override;
        bool skip( size_t _pos ) override;
        size_t tell() const override;
        size_t size() const override;
        bool eof() const override;

    public:
        bool time( uint64_t * const _time ) const override;

    public:
        bool memory( void ** const _memory, size_t * const _size ) override;

    protected:
        bool openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, WChar * const _fullPath );
        bool read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read );
        bool seek_( size_t _pos );

#ifdef MENGINE_DEBUG
    protected:
        const FilePath & getRelationPath() const override;
        const FilePath & getFolderPath() const override;
        const FilePath & getFilePath() const override;
#endif

    protected:
        HANDLE m_hFile;

        size_t m_size;
        size_t m_offset;

        size_t m_carriage;
        size_t m_capacity;
        size_t m_reading;

        uint8_t m_readCache[MENGINE_WIN32_FILE_STREAM_BUFFER_SIZE] = {0};

        MENGINE_THREAD_GUARD_INIT;

#ifdef MENGINE_DEBUG
        FilePath m_relationPath;
        FilePath m_folderPath;
        FilePath m_filePath;
#endif

        bool m_streaming;
        bool m_share;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FileInputStream> Win32FileInputStreamPtr;
    //////////////////////////////////////////////////////////////////////////
}
