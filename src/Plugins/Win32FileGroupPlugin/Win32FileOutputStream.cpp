#include "Win32FileOutputStream.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileOutputStream::Win32FileOutputStream()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileOutputStream::~Win32FileOutputStream()
    {
        if( m_hFile != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_hFile );
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
#ifdef MENGINE_DEBUG
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;
#endif

        WChar fullPath[MENGINE_MAX_PATH] = { L'\0' };
        size_t fullPathLen = Helper::Win32ConcatenateFilePathW( _relationPath, _folderPath, _filePath, fullPath, MENGINE_MAX_PATH );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL_RETURN( fullPathLen != MENGINE_PATH_INVALID_LENGTH, false, "invlalid concatenate filePath '%s':'%s'"
            , _folderPath.c_str()
            , _filePath.c_str()
        );

        HANDLE hFile = Helper::Win32CreateFile(
            fullPath
            , GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , CREATE_ALWAYS
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "invalid open '%ls'"
                , fullPath
            );

            return false;
        }

        m_hFile = hFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileOutputStream::write( const void * _data, size_t _size )
    {
        DWORD bytesWritten = 0;
        BOOL result = ::WriteFile( m_hFile, _data, (DWORD)_size, &bytesWritten, NULL );

        if( result == FALSE )
        {
            LOGGER_ERROR( "write invalid %d"
                , _size
            );

            return 0;
        }

        m_size += bytesWritten;

        return (size_t)bytesWritten;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::flush()
    {
        if( ::FlushFileBuffers( m_hFile ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
