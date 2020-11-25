#include "Win32FileOutputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Win32FileHelper.h"

#include "Kernel/Assertion.h"
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
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        WChar fullPathTemp[MENGINE_MAX_PATH] = {L'\0'};
        size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempW( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH - 1 );

        MENGINE_UNUSED( fullPathTempLen );

        MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s':'%s'"
            , m_folderPath.c_str()
            , m_filePath.c_str()
        );

        HANDLE hFile = Helper::Win32CreateFile(
            fullPathTemp
            , GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , CREATE_ALWAYS
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid open '%ls' [error %lu]"
                , fullPathTemp
                , error
            );

            return false;
        }

        m_hFile = hFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::close()
    {
        if( m_hFile == INVALID_HANDLE_VALUE )
        {
            return true;
        }

        bool successful = this->flush();

        ::CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;

        Char fullPathTemp[MENGINE_MAX_PATH] = {'\0'};
        size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempA( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH - 1 );

        MENGINE_UNUSED( fullPathTempLen );

        MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s':'%s'"
            , m_folderPath.c_str()
            , m_filePath.c_str()
        );

        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, m_filePath, fullPath, MENGINE_MAX_PATH - 1 );

        MENGINE_UNUSED( fullPathLen );

        MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s':'%s'"
            , m_folderPath.c_str()
            , m_filePath.c_str()
        );

        if( PLATFORM_SERVICE()
            ->moveFile( fullPathTemp, fullPath ) == false )
        {
            return false;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileOutputStream::write( const void * _data, size_t _size )
    {
        DWORD bytesWritten = 0;
        if( ::WriteFile( m_hFile, _data, (DWORD)_size, &bytesWritten, NULL ) == FALSE )
        {
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid write %zu [error %lu]"
                , _size
                , error
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
            DWORD error = ::GetLastError();

            LOGGER_ERROR( "invalid flush [error %lu]"
                , error
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
