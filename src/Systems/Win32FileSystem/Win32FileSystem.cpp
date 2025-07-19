#include "Win32FileSystem.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"
#include "Environment/Windows/Win32FileHelper.h"

#include "Win32FileGroupDirectoryFactory.h"
#include "Win32FileGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Path.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::Win32FileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileSystem::Win32FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileSystem::~Win32FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Path currentPath = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::existDirectory( const Char * _basePath, const Char * _directory ) const
    {
        MENGINE_ASSERTION_FATAL( StdString::strlen( _basePath ) == 0
            || (StdString::strrchr( _basePath, '.' ) > StdString::strrchr( _basePath, MENGINE_PATH_FORWARDSLASH )
                || _basePath[StdString::strlen( _basePath ) - 1] == MENGINE_PATH_FORWARDSLASH
                || _basePath[StdString::strlen( _basePath ) - 1] == MENGINE_PATH_BACKSLASH)
            , "invalid path '%s'", _basePath
        );

        MENGINE_ASSERTION_FATAL( StdString::strlen( _directory ) == 0
            || (StdString::strrchr( _directory, '.' ) > StdString::strrchr( _directory, MENGINE_PATH_FORWARDSLASH )
                || _directory[StdString::strlen( _directory ) - 1] == MENGINE_PATH_FORWARDSLASH
                || _directory[StdString::strlen( _directory ) - 1] == MENGINE_PATH_BACKSLASH)
            , "invalid directory '%s'", _directory
        );

        WPath unicode_basePath = {L'\0'};
        if( Helper::utf8ToUnicode( _basePath, unicode_basePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_directory = {L'\0'};
        if( Helper::utf8ToUnicode( _directory, unicode_directory, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = Helper::Win32ExistDirectory( unicode_basePath, unicode_directory );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::createDirectory( const Char * _basePath, const Char * _directory )
    {
        LOGGER_INFO( "file", "create directory path '%s' directory '%s'"
            , _basePath
            , _directory
        );

        MENGINE_ASSERTION_FATAL( StdString::strlen( _basePath ) == 0
            || (StdString::strrchr( _basePath, '.' ) > StdString::strrchr( _basePath, MENGINE_PATH_FORWARDSLASH )
                || _basePath[StdString::strlen( _basePath ) - 1] == MENGINE_PATH_FORWARDSLASH
                || _basePath[StdString::strlen( _basePath ) - 1] == MENGINE_PATH_BACKSLASH)
            , "invalid path '%s'", _basePath
        );

        MENGINE_ASSERTION_FATAL( StdString::strlen( _directory ) == 0
            || (StdString::strrchr( _directory, '.' ) > StdString::strrchr( _directory, MENGINE_PATH_FORWARDSLASH )
                || _directory[StdString::strlen( _directory ) - 1] == MENGINE_PATH_FORWARDSLASH
                || _directory[StdString::strlen( _directory ) - 1] == MENGINE_PATH_BACKSLASH)
            , "invalid directory '%s'", _directory
        );

        WPath unicode_basePath = {L'\0'};
        if( Helper::utf8ToUnicode( _basePath, unicode_basePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_directory = {L'\0'};
        if( Helper::utf8ToUnicode( _directory, unicode_directory, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = Helper::Win32CreateDirectory( unicode_basePath, unicode_directory );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::existFile( const Char * _filePath )
    {
        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        bool result = Helper::Win32ExistFile( unicode_path );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::removeFile( const Char * _filePath )
    {
        LOGGER_INFO( "file", "remove file '%s'"
            , _filePath
        );

        WPath unicode_filePath = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_correctFilePath = {L'\0'};
        Helper::pathCorrectForwardslashToW( unicode_correctFilePath, unicode_filePath );

        if( ::DeleteFileW( unicode_correctFilePath ) == FALSE )
        {
            LOGGER_ERROR( "invalid DeleteFile '%ls' %ls"
                , unicode_correctFilePath
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        LOGGER_INFO( "file", "move file from '%s' to '%s'"
            , _oldFilePath
            , _newFilePath
        );

        WPath unicode_oldFilePath = {L'\0'};
        if( Helper::utf8ToUnicode( _oldFilePath, unicode_oldFilePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_correctOldFilePath = {L'\0'};
        Helper::pathCorrectForwardslashToW( unicode_correctOldFilePath, unicode_oldFilePath );

        WPath unicode_newFilePath = {L'\0'};
        if( Helper::utf8ToUnicode( _newFilePath, unicode_newFilePath, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_correctNewFilePath = {L'\0'};
        Helper::pathCorrectForwardslashToW( unicode_correctNewFilePath, unicode_newFilePath );

#if defined(MENGINE_DEBUG)
        DWORD oldFileAttributes = ::GetFileAttributesW( unicode_correctOldFilePath );

        if( oldFileAttributes != INVALID_FILE_ATTRIBUTES && (oldFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
        {
            LOGGER_WARNING( "invalid move old file '%ls' it's directory"
                , unicode_correctNewFilePath
            );

            return false;
        }
#endif

        DWORD newFileAttributes = ::GetFileAttributesW( unicode_correctNewFilePath );

        if( newFileAttributes != INVALID_FILE_ATTRIBUTES )
        {
#if defined(MENGINE_DEBUG)
            if( (newFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
            {
                LOGGER_WARNING( "invalid move file '%ls' it's directory"
                    , unicode_correctNewFilePath
                );

                return false;
            }
#endif

            if( ::DeleteFileW( unicode_correctNewFilePath ) == FALSE )
            {
                LOGGER_ERROR( "invalid move file '%ls' %ls"
                    , unicode_correctNewFilePath
                    , Helper::Win32GetLastErrorMessageW()
                );
            }
        }

        if( ::MoveFileW( unicode_correctOldFilePath, unicode_correctNewFilePath ) == FALSE )
        {
            LOGGER_ERROR( "file '%ls' move to '%ls' %ls"
                , unicode_correctOldFilePath
                , unicode_correctNewFilePath
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        WPath unicode_base = {L'\0'};
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_mask = {L'\0'};
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        WPath unicode_fullbase = {L'\0'};
        ::GetFullPathNameW( unicode_base, MENGINE_MAX_PATH, unicode_fullbase, NULL );

        Helper::LambdaListDirectoryFilePath lambda_listdirectory = [_lambda]( const WChar * _filePath )
        {
            Path utf8_filepath = {'\0'};
            if( Helper::unicodeToUtf8( _filePath, utf8_filepath, MENGINE_MAX_PATH ) == false )
            {
                return false;
            }

            FilePath fp = Helper::stringizeFilePath( utf8_filepath );

            bool result = _lambda( fp );

            return result;
        };

        bool stop;
        if( Helper::Win32ListDirectory( unicode_fullbase, unicode_mask, unicode_path, lambda_listdirectory, &stop ) == false )
        {
            return false;
        }

        MENGINE_UNUSED( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t Win32FileSystem::getFileTime( const Char * _filePath ) const
    {
        WPath unicode_filePath = {L'\0'};
        Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH );

        HANDLE handle = ::CreateFileW( unicode_filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        if( handle == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "get file time '%ls' invalid CreateFile %ls"
                , unicode_filePath
                , Helper::Win32GetLastErrorMessageW()
            );

            return 0U;
        }

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        BOOL result = ::GetFileTime( handle, &creation, &access, &write );

        if( result == FALSE )
        {
            ::CloseHandle( handle );

            LOGGER_ERROR( "get file time '%ls' invalid GetFileTime %ls"
                , unicode_filePath
                , Helper::Win32GetLastErrorMessageW()
            );

            return 0U;
        }

        ::CloseHandle( handle );

        time_t time = Helper::Win32FileTimeToUnixTime( &write );

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
}