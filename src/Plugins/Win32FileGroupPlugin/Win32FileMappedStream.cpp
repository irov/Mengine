#include "Win32FileMappedStream.h"

#include "Interface/MemoryServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
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
        if( m_memory != NULL )
        {
            if( UnmapViewOfFile( m_memory ) == FALSE )
            {
                DWORD uError = GetLastError();

                LOGGER_ERROR( "invalid UnmapViewOfFile %p error %d"
                    , m_memory
                    , uError
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        WChar concatenatePath[MENGINE_MAX_PATH];
        if( Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, concatenatePath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "invlalid concatenate filePath '%s':'%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        m_hFile = Helper::Win32CreateFile(
            concatenatePath, // file to open
            GENERIC_READ, // open for reading
            FILE_SHARE_READ, // share for reading, exclusive for mapping
            OPEN_EXISTING // existing file only
        );

        if( m_hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "file '%ls' invalid open"
                , concatenatePath
            );

            return false;
        }

        m_hMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

        if( m_hMapping == NULL )
        {
            DWORD error = GetLastError();

            LOGGER_ERROR( "invalid create file mapping '%ls' error %d"
                , concatenatePath
                , error
            );

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        m_memory = MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );

        if( m_memory == NULL )
        {
            DWORD error = GetLastError();

            LOGGER_ERROR( "invalid map view of file '%ls' error %d"
                , concatenatePath
                , error
            );

            ::CloseHandle( m_hMapping );
            m_hMapping = INVALID_HANDLE_VALUE;

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileMappedStream::createFileStream( const Char * _doc )
    {
        MemoryProxyInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxyInput( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMappedStream::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory )
    {
        MemoryProxyInputInterface * memory = stdex::intrusive_get<MemoryProxyInputInterface *>( _stream );

        void * memory_buffer = memory->setBuffer( m_memory, _offset, _size, MENGINE_DOCUMENT_FUNCTION );

        if( _memory != nullptr )
        {
            *_memory = memory_buffer;
        }

        return true;
    }
}
