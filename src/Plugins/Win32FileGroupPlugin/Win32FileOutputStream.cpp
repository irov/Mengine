#include "Win32FileOutputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32Helper.h"

#include "Win32ConcatenateFileHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileOutputStream::Win32FileOutputStream()
        : m_hFile( INVALID_HANDLE_VALUE )
        , m_size( 0 )
        , m_withTemp( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileOutputStream::~Win32FileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_withTemp = _withTemp;

        WChar fullPathTemp[MENGINE_MAX_PATH] = {L'\0'};

        if( m_withTemp == true )
        {
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempW( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );
        }
        else
        {
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathW( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );
        }

        HANDLE hFile = Helper::Win32CreateFile(
            fullPathTemp
            , GENERIC_WRITE
            , FILE_SHARE_READ | FILE_SHARE_WRITE
            , CREATE_ALWAYS
        );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "invalid open '%ls'"
                , fullPathTemp
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

        if( m_withTemp == true )
        {
            Char fullPathTemp[MENGINE_MAX_PATH] = {'\0'};
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempA( m_relationPath, m_folderPath, m_filePath, fullPathTemp, MENGINE_MAX_PATH );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            Char fullPath[MENGINE_MAX_PATH] = {'\0'};
            size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, m_filePath, fullPath, MENGINE_MAX_PATH );

            MENGINE_UNUSED( fullPathLen );

            MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s':'%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            if( PLATFORM_SERVICE()
                ->moveFile( fullPathTemp, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid move close file from '%s' to '%s'"
                    , fullPathTemp
                    , fullPath
                );

                return false;
            }
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileOutputStream::write( const void * _data, size_t _size )
    {
        DWORD bytesWritten = 0;
        if( ::WriteFile( m_hFile, _data, (DWORD)_size, &bytesWritten, NULL ) == FALSE )
        {
            LOGGER_ERROR( "invalid write %zu get error %ls"
                , _size
                , Helper::Win32GetLastErrorMessage()
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
            LOGGER_ERROR( "invalid FlushFileBuffers get error %ls"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HANDLE Win32FileOutputStream::getHandleFile() const
    {
        return m_hFile;
    }
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
//////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileOutputStream::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileOutputStream::getFolderPath() const
    {
        return m_folderPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Win32FileOutputStream::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
