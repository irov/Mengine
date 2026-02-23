#include "SDL3FileSystem.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/WindowsIncluder.h"
#   include "Environment/Windows/Win32FileHelper.h"
#elif defined(MENGINE_VENDOR_APPLE)
#   if defined(MENGINE_PLATFORM_MACOS)
#       include "Environment/MacOS/MacOSUtils.h"
#   endif
#endif

#include "SDL3FileGroupDirectory.h"
#include "SDL3FileGroupDirectoryFactory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Path.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::SDL3FileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool createDirectoryFullpath( const Char * _fullpath )
        {
#if defined(MENGINE_PLATFORM_MACOS)
            int status = ::mkdir( _fullpath, S_IRWXU );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' already exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_LINUX)
            int status = ::mkdir( _fullpath, S_IRWXU );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' already exists"
                    , _fullpath
                );

                return false;
            }

#elif defined(MENGINE_PLATFORM_WINDOWS)
            WPath unicode_fullpath = {L'\0'};
            Helper::utf8ToUnicode( _fullpath, unicode_fullpath, MENGINE_MAX_PATH - 1 );

            BOOL successful = ::CreateDirectoryW( unicode_fullpath, NULL );

            if( successful == FALSE )
            {
                DWORD err = GetLastError();

                switch( err )
                {
                case ERROR_ALREADY_EXISTS:
                    {
                        return true;
                    }break;
                case ERROR_PATH_NOT_FOUND:
                    {
                        LOGGER_WARNING( "'%s' not found"
                            , _fullpath
                        );

                        return false;
                    }break;
                default:
                    {
                        LOGGER_WARNING( "'%s' unknown error %d"
                            , _fullpath
                            , err
                        );
                    }break;
                }

                return false;
            }
#endif

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isDirectoryFullpath( const Char * _fullpath )
        {
            struct stat sb;
            int err = ::stat( _fullpath, &sb );

            if( err != 0 )
            {
                return false;
            }

            if( (sb.st_mode & S_IFMT) == S_IFDIR )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileSystem::SDL3FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileSystem::~SDL3FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<SDL3FileGroupDirectoryFactory, SDL3FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Path utf8_currentPath = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );
        
#if defined(MENGINE_PLATFORM_IOS)
        Helper::pathAppendA( utf8_currentPath, "Data/", MENGINE_PATH_FORWARDSLASH );
#elif defined(MENGINE_PLATFORM_MACOS)
        Helper::pathAppendA( utf8_currentPath, "Data/", MENGINE_PATH_FORWARDSLASH );
#endif
        
        size_t utf8_currentPathLen = StdString::strlen( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, (FilePath::size_type)utf8_currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<SDL3FileGroupDirectoryFactory, SDL3FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::existDirectory( const Char * _basePath, const Char * _directory ) const
    {
        Path pathDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory, MENGINE_PATH_FORWARDSLASH );

        size_t len = StdString::strlen( pathDirectory );

        if( len == 0 )	// current dir
        {
            return true;
        }

        Path pathFull = {'\0'};
        MENGINE_SNPRINTF( pathFull, MENGINE_MAX_PATH, "%s%s", _basePath, pathDirectory );

        if( pathFull[len - 1] == ':' )	// root dir
        {
            return true;	// let it be
        }
#if defined(MENGINE_PLATFORM_MACOS)
        else if( pathFull[len - 1] == '~' )	// root dir
        {
            return true;	// let it be
        }
#endif

        bool exist = Detail::isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::createDirectory( const Char * _basePath, const Char * _directory )
    {
        size_t directorySize = StdString::strlen( _directory );

        if( directorySize == 0 )
        {
            return true;
        }

        Path pathDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory, MENGINE_PATH_FORWARDSLASH );

        if( StdString::strlen( pathDirectory ) == 0 )
        {
            return true;
        }

        Path pathTestDirectory = {'\0'};
        Helper::pathCombineA( pathTestDirectory, _basePath, pathDirectory, MENGINE_PATH_FORWARDSLASH );

        if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
        {
            return true;
        }

        Helper::pathRemoveSlashA( pathDirectory, MENGINE_PATH_FORWARDSLASH );

        uint32_t paths_count = 0;
        Path paths[16];

        for( ;; )
        {
            StdString::strcpy_safe( paths[paths_count++], pathDirectory, MENGINE_MAX_PATH );

            if( Helper::pathRemoveFileSpecA( pathDirectory, MENGINE_PATH_FORWARDSLASH ) == false )
            {
                break;
            }

            if( StdString::strlen( _basePath ) == 0 )
            {
                break;
            }

            Helper::pathRemoveSlashA( pathDirectory, MENGINE_PATH_FORWARDSLASH );

            Helper::pathCombineA( pathTestDirectory, _basePath, pathDirectory, MENGINE_PATH_FORWARDSLASH );

            if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( uint32_t index = paths_count; index != 0; --index )
        {
            const Char * path = paths[index - 1];

            Path pathCreateDirectory = {'\0'};
            Helper::pathCombineA( pathCreateDirectory, _basePath, path, MENGINE_PATH_FORWARDSLASH );

            if( Detail::createDirectoryFullpath( pathCreateDirectory ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::existFile( const Char * _filePath )
    {
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        // Use SDL_GetPathInfo for proper file existence check in SDL3
        // Pass NULL as info to only check for existence without getting file info
        bool exists = SDL_GetPathInfo( pathCorrect, NULL );

        return exists;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::removeFile( const Char * _filePath )
    {
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        int result = ::remove( pathCorrect );

        if( result != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        Path oldPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Path newPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

        struct stat sb;
        if( stat( newPathCorrect, &sb ) == 0 && ((sb.st_mode) & S_IFMT) != S_IFDIR )
        {
            int result_remove = ::remove( newPathCorrect );

            if( result_remove != 0 )
            {
                const char * msg = ::strerror( errno );

                LOGGER_ASSERTION( "invalid remove new move file from '%s' to '%s' error '%s' [%u]"
                    , _oldFilePath
                    , _newFilePath
                    , msg
                    , errno
                );
            }
        }

        int result_rename = ::rename( oldPathCorrect, newPathCorrect );

        if( result_rename != 0 )
        {
            const char * msg = ::strerror( errno );

            LOGGER_ASSERTION( "invalid move file from '%s' to '%s' error '%s' [%u]"
                , _oldFilePath
                , _newFilePath
                , msg
                , errno
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool listDirectoryContents( const WChar * _dir, const WChar * _mask, const WChar * _path, const LambdaFilePath & _lambda, bool * const _stop )
        {
            WPath sDir = {L'\0'};
            ::PathCanonicalizeW( sDir, _dir );

            {
                WPath sPath = {L'\0'};
                StdString::wcscpy_safe( sPath, sDir, MENGINE_MAX_PATH );
                StdString::wcscat_safe( sPath, _path, MENGINE_MAX_PATH );
                StdString::wcscat_safe( sPath, _mask, MENGINE_MAX_PATH );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind != INVALID_HANDLE_VALUE )
                {
                    do
                    {
                        if( StdString::wcscmp( fdFile.cFileName, L"." ) == 0 ||
                            StdString::wcscmp( fdFile.cFileName, L".." ) == 0 )
                        {
                            continue;
                        }

                        if( fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                        {
                            continue;
                        }

                        WPath sPath2 = {L'\0'};
                        StdString::wcscpy_safe( sPath2, sPath, MENGINE_MAX_PATH );
                        StdString::wcscat_safe( sPath2, L"\0", MENGINE_MAX_PATH );

                        Helper::pathCorrectForwardslashW( sPath2 );

                        ::PathRemoveFileSpec( sPath2 );

                        WPath unicode_filepath = {L'\0'};
                        ::PathCombine( unicode_filepath, sPath2, fdFile.cFileName );

                        WPath unicode_out = {L'\0'};
                        if( StdString::wcslen( sDir ) != 0 )
                        {
                            ::PathRelativePathTo( unicode_out
                                , sDir
                                , FILE_ATTRIBUTE_DIRECTORY
                                , unicode_filepath
                                , FILE_ATTRIBUTE_NORMAL );
                        }
                        else
                        {
                            StdString::wcscpy_safe( unicode_out, unicode_filepath, MENGINE_MAX_PATH );
                        }

                        Path utf8_filepath = {'\0'};
                        if( Helper::unicodeToUtf8( unicode_out, utf8_filepath, MENGINE_MAX_PATH ) == false )
                        {
                            ::FindClose( hFind );

                            return false;
                        }

                        FilePath fp = Helper::stringizeFilePath( utf8_filepath );

                        if( _lambda( fp ) == false )
                        {
                            ::FindClose( hFind );

                            *_stop = true;

                            return true;
                        }

                    } while( ::FindNextFile( hFind, &fdFile ) != FALSE );
                }

                ::FindClose( hFind );
            }

            {
                WPath sPath = {L'\0'};
                StdString::wcscpy_safe( sPath, sDir, MENGINE_MAX_PATH );
                StdString::wcscat_safe( sPath, _path, MENGINE_MAX_PATH );
                StdString::wcscat_safe( sPath, L"*.*", MENGINE_MAX_PATH );

                WIN32_FIND_DATA fdFile;
                HANDLE hFind = ::FindFirstFileEx( sPath, FindExInfoStandard, &fdFile, FindExSearchNameMatch, NULL, 0 );

                if( hFind == INVALID_HANDLE_VALUE )
                {
                    return true;
                }

                do
                {
                    if( StdString::wcscmp( fdFile.cFileName, L"." ) == 0 ||
                        StdString::wcscmp( fdFile.cFileName, L".." ) == 0 )
                    {
                        continue;
                    }

                    if( (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
                    {
                        continue;
                    }

                    WPath currentPath = {L'\0'};
                    StdString::wcscpy_safe( currentPath, sPath, MENGINE_MAX_PATH );
                    StdString::wcscat_safe( currentPath, L"\0", MENGINE_MAX_PATH );

                    ::PathRemoveFileSpec( currentPath );

                    WPath nextPath = {L'\0'};
                    ::PathCombine( nextPath, currentPath, fdFile.cFileName );

                    StdString::wcscat_safe( nextPath, L"\\", MENGINE_MAX_PATH );

                    bool stop;
                    if( Detail::listDirectoryContents( sDir, _mask, nextPath, _lambda, &stop ) == false )
                    {
                        ::FindClose( hFind );

                        return false;
                    }

                    if( stop == true )
                    {
                        ::FindClose( hFind );

                        *_stop = true;

                        return true;
                    }

                } while( ::FindNextFile( hFind, &fdFile ) != FALSE );

                ::FindClose( hFind );
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileSystem::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _base );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        WPath unicode_base = {L'\0'};
        if( Helper::utf8ToUnicode( _base, unicode_base, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        WPath unicode_mask = {L'\0'};
        if( Helper::utf8ToUnicode( _mask, unicode_mask, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        WPath unicode_path = {L'\0'};
        if( Helper::utf8ToUnicode( _path, unicode_path, MENGINE_MAX_PATH - 1 ) == false )
        {
            return false;
        }

        WPath unicode_fullbase = {L'\0'};
        ::GetFullPathName( unicode_base, MENGINE_MAX_PATH, unicode_fullbase, NULL );

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
#else
        LOGGER_WARNING( "findFiles not support" );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t SDL3FileSystem::getFileTime( const Char * _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        WPath unicode_filePath = {L'\0'};
        if( Helper::utf8ToUnicode( _filePath, unicode_filePath, MENGINE_MAX_PATH - 1 ) == false )
        {
            return 0U;
        }

        HANDLE handle = ::CreateFile( unicode_filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

        FILETIME creation;
        FILETIME access;
        FILETIME write;

        if( ::GetFileTime( handle, &creation, &access, &write ) == FALSE )
        {
            ::CloseHandle( handle );

            return 0U;
        }

        ::CloseHandle( handle );

        time_t time = Helper::Win32FileTimeToUnixTime( &write );

        return time;
#else
        return 0U;
#endif		
    }
    //////////////////////////////////////////////////////////////////////////
}

