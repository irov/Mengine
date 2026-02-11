#include "Win32FileMapped.h"

#include "Interface/MemoryServiceInterface.h"

#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32Helper.h"

#include "Win32ConcatenateFileHelper.h"
#include "Win32FileMappedInputStream.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileMapped::Win32FileMapped()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_hMapping( INVALID_HANDLE_VALUE )
        , m_liSize()
        , m_dwAllocationGranularity( 0 )
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
        WPath fullPath = {L'\0'};
        size_t fullPathLen = Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, fullPath );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s%s%s'"
            , _relationPath.c_str()
            , _folderPath.c_str()
            , _filePath.c_str()
        );

        DWORD sharedMode = FILE_SHARE_READ;

        if( _share == true )
        {
            sharedMode |= FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }

        HANDLE hFile = Helper::Win32CreateFile( fullPath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING );

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
            LOGGER_ERROR( "invalid file '%ls' size get error %ls"
                , fullPath
                , Helper::Win32GetLastErrorMessageW()
            );

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        HANDLE hMapping = ::CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );

        if( hMapping == NULL )
        {
            LOGGER_ERROR( "invalid create file mapping '%ls' get error %ls"
                , fullPath
                , Helper::Win32GetLastErrorMessageW()
            );

            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;

            return false;
        }

        m_hMapping = hMapping;

        SYSTEM_INFO si;
        ::GetSystemInfo( &si );

        m_dwAllocationGranularity = si.dwAllocationGranularity;

        m_factoryFileMappedInputStream = Helper::makeFactoryPoolWithMutex<Win32FileMappedInputStream, 128>( MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileMapped::close()
    {
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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryFileMappedInputStream );

        m_factoryFileMappedInputStream = nullptr;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Win32FileMapped::createInputStream( const DocumentInterfacePtr & _doc )
    {
        Win32FileMappedInputStreamPtr stream = m_factoryFileMappedInputStream->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input stream" );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileMapped::openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        Win32FileMappedInputStream * stream = _stream.getT<Win32FileMappedInputStream *>();

        size_t size = _size == MENGINE_UNKNOWN_SIZE ? (size_t)m_liSize.QuadPart : _size;

        bool result = stream->mapViewOfFile( m_hMapping, m_dwAllocationGranularity, _offset, size );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
