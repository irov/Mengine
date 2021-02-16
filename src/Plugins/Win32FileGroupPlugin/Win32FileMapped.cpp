#include "Win32FileMapped.h"

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
    Win32FileMapped::Win32FileMapped()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_hMapping( INVALID_HANDLE_VALUE )
        , m_memory( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileMapped::~Win32FileMapped()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMapped::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share )
    {
        WChar fullPath[MENGINE_MAX_PATH] = {L'\0'};
        size_t fullPathLen = Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s':'%s'"
            , _folderPath.c_str()
            , _filePath.c_str()
        );

        DWORD sharedMode = FILE_SHARE_READ;

        if( _share == true )
        {
            sharedMode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }

        HANDLE hFile = Helper::Win32CreateFile(
            fullPath, // file to open
            GENERIC_READ, // open for reading
            FILE_SHARE_READ, // share for reading, exclusive for mapping
            OPEN_EXISTING // existing file only
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "file '%ls' invalid open"
                , fullPath
            );

            return false;
        }

        m_hFile = hFile;

        if( ::GetFileSizeEx( m_hFile, &m_liSize ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid file '%ls' size [error: %lu]"
                , fullPath
                , error
            );

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        //size_t size = (size_t)lpFileSize.QuadPart;

        HANDLE hMapping = ::CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

        if( hMapping == NULL )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid create file mapping '%ls' [error: %lu]"
                , fullPath
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
                , fullPath
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
    bool Win32FileMapped::close()
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
    InputStreamInterfacePtr Win32FileMapped::createInputStream( const DocumentPtr & _doc )
    {
        MemoryProxyInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxyInput( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMapped::openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** const _memory )
    {
        MemoryProxyInputInterface * memory = stdex::intrusive_get<MemoryProxyInputInterface *>( _stream );

        size_t size = _size == ~0U ? (size_t)m_liSize.QuadPart : _size;

        void * memory_buffer = memory->setBuffer( m_memory, _offset, size );

        if( _memory != nullptr )
        {
            *_memory = memory_buffer;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
