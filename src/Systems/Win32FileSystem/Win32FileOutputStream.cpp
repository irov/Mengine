#include "Win32FileOutputStream.h"

#include "Interface/FileSystemInterface.h"

#include "Environment/Windows/Win32FileHelper.h"
#include "Environment/Windows/Win32Helper.h"

#include "Win32ConcatenateFileHelper.h"

#include "Kernel/NotificationHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/Path.h"

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

        WPath fullPathTemp = {L'\0'};

        if( m_withTemp == true )
        {
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempW( m_relationPath, m_folderPath, m_filePath, fullPathTemp );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s%s%s'"
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );
        }
        else
        {
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathW( m_relationPath, m_folderPath, m_filePath, fullPathTemp );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s%s%s'"
                , m_relationPath.c_str()
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
            LOGGER_ERROR( "invalid open output relation '%s' folder '%s' file '%s' full '%ls'"
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , fullPathTemp
            );

            return false;
        }

        m_hFile = hFile;

        STATISTIC_INC_INTEGER( STATISTIC_FILE_OPEN_COUNT );

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, m_folderPath, m_filePath, false, false );
        }
#endif

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileOutputStream::close()
    {
        if( m_hFile == INVALID_HANDLE_VALUE )
        {
            return true;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, m_folderPath, m_filePath, false, false );
        }
#endif

        bool successful = this->flush();

        ::CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;

        if( m_withTemp == true )
        {
            Path fullPathTemp = {'\0'};
            size_t fullPathTempLen = Helper::Win32ConcatenateFilePathTempA( m_relationPath, m_folderPath, m_filePath, fullPathTemp );

            MENGINE_UNUSED( fullPathTempLen );

            MENGINE_ASSERTION_FATAL( fullPathTempLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePathTemp '%s%s%s'"
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            Path fullPath = {'\0'};
            size_t fullPathLen = Helper::Win32ConcatenateFilePathA( m_relationPath, m_folderPath, m_filePath, fullPath );

            MENGINE_UNUSED( fullPathLen );

            MENGINE_ASSERTION_FATAL( fullPathLen != MENGINE_PATH_INVALID_LENGTH, "invlalid concatenate filePath '%s%s%s'"
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            if( FILE_SYSTEM()
                ->moveFile( fullPathTemp, fullPath ) == false )
            {
                LOGGER_ERROR( "invalid move close file from '%s' to '%s'"
                    , fullPathTemp
                    , fullPath
                );

                return false;
            }
        }

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32FileOutputStream::write( const void * _data, size_t _size )
    {
        DWORD bytesWritten = 0;
        if( ::WriteFile( m_hFile, _data, (DWORD)_size, &bytesWritten, NULL ) == FALSE )
        {
            LOGGER_ERROR( "invalid write %zu filePath '%s%s%s' get error %ls"
                , _size
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , Helper::Win32GetLastErrorMessageW()
            );

            return 0;
        }

        m_size += (size_t)bytesWritten;

        STATISTIC_ADD_INTEGER( STATISTIC_FILE_WRITE_BYTES, bytesWritten );

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
            LOGGER_ERROR( "invalid FlushFileBuffers filePath '%s%s%s' get error %ls"
                , m_relationPath.c_str()
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , Helper::Win32GetLastErrorMessageW()
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
}
