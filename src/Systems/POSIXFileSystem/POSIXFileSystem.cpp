#include "POSIXFileSystem.h"

#include "Interface/PlatformServiceInterface.h"

#include "POSIXFileGroupDirectory.h"
#include "POSIXFileGroupDirectoryFactory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/String.h"

#include "Config/StdString.h"
#include "Config/Path.h"

#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
#include <sys/stat.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::POSIXFileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool createDirectoryFullpath( const Char * _fullpath )
        {
            int status = ::mkdir( _fullpath, S_IRWXU );

            if( status == 0 )
            {
                return true;
            }

            if( errno == EEXIST )
            {
                struct stat existing;

                if( ::stat( _fullpath, &existing ) == 0 && S_ISDIR( existing.st_mode ) != 0 )
                {
                    return true;
                }

                LOGGER_ERROR( "can't create directory '%s': path exists and is not a directory"
                    , _fullpath
                );

                return false;
            }

            LOGGER_ERROR( "invalid create directory '%s' error: %d"
                , _fullpath
                , errno
            );

            return false;
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

            if( (sb.st_mode & S_IFMT) != S_IFDIR )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isFileFullpath( const Char * _fullpath )
        {
            struct stat sb;
            int err = ::stat( _fullpath, &sb );

            if( err != 0 )
            {
                return false;
            }

            if( (sb.st_mode & S_IFMT) == S_IFDIR )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static String concatenatePath( const String & _base, const String & _path )
        {
            if( _base.empty() == true )
            {
                return _path;
            }

            if( _path.empty() == true )
            {
                return _base;
            }

            String result = _base;

            if( result.back() != MENGINE_PATH_FORWARDSLASH && _path.front() != MENGINE_PATH_FORWARDSLASH )
            {
                result.push_back( MENGINE_PATH_FORWARDSLASH );
            }

            result.append( _path );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool listDirectoryContents( const String & _base, const String & _path, const Char * _mask, const LambdaFilePath & _lambda, bool * const _stop )
        {
            String fullPath = Detail::concatenatePath( _base, _path );

            if( fullPath.empty() == true )
            {
                fullPath.assign( "." );
            }

            DIR * directory = ::opendir( fullPath.c_str() );

            if( directory == nullptr )
            {
                if( errno == ENOENT || errno == ENOTDIR )
                {
                    *_stop = false;

                    return true;
                }

                LOGGER_ERROR( "invalid open directory '%s' error: %d"
                    , fullPath.c_str()
                    , errno
                );

                return false;
            }

            for( ;; )
            {
                errno = 0;

                dirent * entry = ::readdir( directory );

                if( entry == nullptr )
                {
                    int readError = errno;

                    ::closedir( directory );

                    if( readError != 0 )
                    {
                        LOGGER_ERROR( "invalid read directory '%s' error: %d"
                            , fullPath.c_str()
                            , readError
                        );

                        return false;
                    }

                    *_stop = false;

                    return true;
                }

                const Char * name = entry->d_name;

                if( StdString::strcmp( name, "." ) == 0 || StdString::strcmp( name, ".." ) == 0 )
                {
                    continue;
                }

                String childPath = Detail::concatenatePath( _path, name );
                String childFullPath = Detail::concatenatePath( _base, childPath );

                struct stat childStat;
                if( ::lstat( childFullPath.c_str(), &childStat ) != 0 )
                {
                    LOGGER_ERROR( "invalid stat path '%s' error: %d"
                        , childFullPath.c_str()
                        , errno
                    );

                    ::closedir( directory );

                    return false;
                }

                if( S_ISDIR( childStat.st_mode ) != 0 )
                {
                    bool recursiveStop;
                    if( Detail::listDirectoryContents( _base, childPath, _mask, _lambda, &recursiveStop ) == false )
                    {
                        ::closedir( directory );

                        return false;
                    }

                    if( recursiveStop == true )
                    {
                        ::closedir( directory );

                        *_stop = true;

                        return true;
                    }

                    continue;
                }

                if( ::fnmatch( _mask, name, 0 ) != 0 )
                {
                    continue;
                }

                FilePath filePath = Helper::stringizeFilePath( childPath.c_str() );

                if( _lambda( filePath ) == false )
                {
                    ::closedir( directory );

                    *_stop = true;

                    return true;
                }
            }

        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXFileSystem::POSIXFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXFileSystem::~POSIXFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXFileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<POSIXFileGroupDirectoryFactory, POSIXFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Path currentPath = {'\0'};
        size_t currentPathSize = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        Helper::pathCorrectFolderPathA( currentPath, MENGINE_PATH_FORWARDSLASH );

        currentPathSize = StdString::strlen( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathSize );

        FactoryInterfacePtr factoryDirectoryFileGroupDirectory = Helper::makeFactory<POSIXFileGroupDirectoryFactory, POSIXFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirectoryFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXFileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXFileSystem::existDirectory( const Char * _basePath, const Char * _directory ) const
    {
        Path correctBasePath = {'\0'};
        Helper::pathCorrectBackslashToA( correctBasePath, _basePath );

        Path correctDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( correctDirectory, _directory );

        size_t correctDirectoryLen = StdString::strlen( correctDirectory );

        if( correctDirectoryLen == 0 )
        {
            return true;
        }

        Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_FORWARDSLASH );

        correctDirectoryLen = StdString::strlen( correctDirectory );

        if( correctDirectoryLen == 0 )
        {
            return true;
        }

        if( correctDirectory[correctDirectoryLen - 1] == ':' )
        {
            return true;
        }

        Path pathFull = {'\0'};
        Helper::pathCombineA( pathFull, correctBasePath, correctDirectory, MENGINE_PATH_FORWARDSLASH );

        bool exist = Detail::isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXFileSystem::createDirectory( const Char * _basePath, const Char * _directory )
    {
        LOGGER_INFO( "file", "create directory path '%s' directory '%s'"
            , _basePath
            , _directory
        );

        Path correctBasePath = {'\0'};
        Helper::pathCorrectBackslashToA( correctBasePath, _basePath );

        Path correctDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( correctDirectory, _directory );

        Helper::pathRemoveFileSpecA( correctDirectory, MENGINE_PATH_FORWARDSLASH );

        size_t correctDirectoryLen = StdString::strlen( correctDirectory );

        if( correctDirectoryLen == 0 )
        {
            return true;
        }

        Path pathTestDirectory = {'\0'};
        Helper::pathCombineA( pathTestDirectory, correctBasePath, correctDirectory, MENGINE_PATH_FORWARDSLASH );

        if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
        {
            return true;
        }

        Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_FORWARDSLASH );

        uint32_t paths_count = 0;
        Path paths[64];

        for( ;; )
        {
            if( paths_count >= 64 )
            {
                LOGGER_ERROR( "too many directories in path '%s'"
                    , correctDirectory
                );

                return false;
            }

            StdString::strcpy_safe( paths[paths_count++], correctDirectory, MENGINE_MAX_PATH );

            if( Helper::pathRemoveFileSpecA( correctDirectory, MENGINE_PATH_FORWARDSLASH ) == false )
            {
                break;
            }

            Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_FORWARDSLASH );

            Helper::pathCombineA( pathTestDirectory, correctBasePath, correctDirectory, MENGINE_PATH_FORWARDSLASH );

            if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( uint32_t index = paths_count; index != 0; --index )
        {
            const Char * path = paths[index - 1];

            Path pathCreateDirectory = {'\0'};
            Helper::pathCombineA( pathCreateDirectory, correctBasePath, path, MENGINE_PATH_FORWARDSLASH );

            if( Detail::createDirectoryFullpath( pathCreateDirectory ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXFileSystem::existFile( const Char * _filePath )
    {
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        bool exist = Detail::isFileFullpath( pathCorrect );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXFileSystem::removeFile( const Char * _filePath )
    {
        LOGGER_INFO( "file", "remove file '%s'"
            , _filePath
        );

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
    bool POSIXFileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        LOGGER_INFO( "file", "move file from '%s' to '%s'"
            , _oldFilePath
            , _newFilePath
        );

        Path oldPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Path newPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

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
    bool POSIXFileSystem::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        Path correctBase = {'\0'};
        Helper::pathCorrectBackslashToA( correctBase, _base );

        Path correctPath = {'\0'};
        Helper::pathCorrectBackslashToA( correctPath, _path );

        Path correctMask = {'\0'};
        Helper::pathCorrectBackslashToA( correctMask, _mask );

        const Char * path = correctPath;
        while( *path == MENGINE_PATH_FORWARDSLASH )
        {
            ++path;
        }

        bool stop;
        if( Detail::listDirectoryContents( correctBase, path, correctMask, _lambda, &stop ) == false )
        {
            return false;
        }

        MENGINE_UNUSED( stop );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t POSIXFileSystem::getFileTime( const Char * _filePath ) const
    {
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        struct stat fs;
        if( ::stat( pathCorrect, &fs ) != 0 )
        {
            return 0U;
        }

        return fs.st_mtime;
    }
    //////////////////////////////////////////////////////////////////////////
}
