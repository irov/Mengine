#include "Win32FileMappedStream.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileMappedStream::Win32FileMappedStream()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_hMapping( INVALID_HANDLE_VALUE )
        , m_memory( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileMappedStream::~Win32FileMappedStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        WChar concatenatePath[MENGINE_MAX_PATH] = {L'\0'};
        size_t fullPathLen = Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, concatenatePath, MENGINE_MAX_PATH - 1 );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s':'%s'"
            , _folderPath.c_str()
            , _filePath.c_str()
        );

        HANDLE hFile = Helper::Win32CreateFile(
            concatenatePath, // file to open
            GENERIC_READ, // open for reading
            FILE_SHARE_READ, // share for reading, exclusive for mapping
            OPEN_EXISTING // existing file only
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "file '%ls' invalid open [error: %lu]"
                , concatenatePath
                , error
            );

            return false;
        }

        m_hFile = hFile;

        HANDLE hMapping = ::CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

        if( hMapping == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid create file mapping '%ls' [error: %lu]"
                , concatenatePath
                , error
            );

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        m_hMapping = hMapping;

        LPVOID memory = ::MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );

        if( memory == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid map view of file '%ls' [error: %lu]"
                , concatenatePath
                , error
            );

            ::CloseHandle( m_hMapping );
            m_hMapping = INVALID_HANDLE_VALUE;

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedStream::close()
    {
        if( m_memory != NULL )
        {
            if( ::UnmapViewOfFile( m_memory ) == FALSE )
            {
                DWORD error = ::GetLastError();

                LOGGER_ERROR( "invalid UnmapViewOfFile %p [error: %lu]"
                    , m_memory
                    , error
                );
            }

            m_memory = NULL;
        }

        if( m_hMapping != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hMapping );
            m_hMapping = INVALID_HANDLE_VALUE;
        }

        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileMappedStream::createFileStream( const DocumentPtr & _doc )
    {
        MemoryProxyInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxyInput( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedStream::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** const _memory )
    {
        MemoryProxyInputInterface * memory = stdex::intrusive_get<MemoryProxyInputInterface *>( _stream );

        void * memory_buffer = memory->setBuffer( m_memory, _offset, _size );

        if( _memory != nullptr )
        {
            *_memory = memory_buffer;
        }

        return true;
    }
}
