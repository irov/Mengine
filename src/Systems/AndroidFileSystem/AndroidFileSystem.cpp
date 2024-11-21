#include "AndroidFileSystem.h"

#include "AndroidFileGroupDirectory.h"
#include "AndroidAssetGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Path.h"

#include <sys/stat.h>
#include <dlfcn.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::AndroidFileSystem );
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

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' already exists"
                    , _fullpath
                );

                return false;
            }

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
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidFileSystem::AndroidFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidFileSystem::~AndroidFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalDirGroupDirectory = Helper::makeFactoryDefault<AndroidFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalDirGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        FactoryInterfacePtr factoryAssetGroupDirectory = Helper::makeFactoryDefault<AndroidAssetGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryAssetGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidFileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::existDirectory( const Char * _basePath, const Char * _directory ) const
    {
        Path correctBasePath = {'\0'};
        Helper::pathCorrectBackslashToA( correctBasePath, _basePath );

        Path correctDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( correctDirectory, _directory );

        Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        size_t correctDirectoryLen = StdString::strlen( correctDirectory );

        if( correctDirectoryLen == 0 )
        {
            return true;
        }

        if( correctDirectory[correctDirectoryLen - 1] == ':' )
        {
            return true;
        }

        Path pathFull = {'\0'};
        Helper::pathCombineA( pathFull, correctBasePath, correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        bool exist = Detail::isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::createDirectory( const Char * _basePath, const Char * _directory )
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

        Helper::pathRemoveFileSpecA( correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        if( StdString::strlen( correctDirectory ) == 0 )
        {
            return true;
        }

        Path pathTestDirectory = {'\0'};
        Helper::pathCombineA( pathTestDirectory, correctBasePath, correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
        {
            return true;
        }

        Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        uint32_t paths_count = 0;
        Path paths[16];

        for( ;; )
        {
            StdString::strncpy( paths[paths_count++], correctDirectory, MENGINE_MAX_PATH );

            if( Helper::pathRemoveFileSpecA( correctDirectory, MENGINE_PATH_DELIM_BACKSLASH ) == false )
            {
                break;
            }

            Helper::pathRemoveSlashA( correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

            Helper::pathCombineA( pathTestDirectory, correctBasePath, correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

            if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( uint32_t index = paths_count; index != 0; --index )
        {
            const Char * path = paths[index - 1];

            Path pathCreateDirectory = {'\0'};
            Helper::pathCombineA( pathCreateDirectory, correctBasePath, path, MENGINE_PATH_DELIM_BACKSLASH );

            if( Detail::createDirectoryFullpath( pathCreateDirectory ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::existFile( const Char * _filePath )
    {
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        bool exist = Detail::isFileFullpath( pathCorrect );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::removeFile( const Char * _filePath )
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
    bool AndroidFileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
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
    bool AndroidFileSystem::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _base );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

        LOGGER_WARNING( "AndroidPlatformService::findFiles not support" );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t AndroidFileSystem::getFileTime( const Char * _filePath ) const
    {
        struct stat fs;
        if( stat( _filePath, &fs ) != 0 )
        {
            return 0U;
        }

        return fs.st_mtime;
    }
    //////////////////////////////////////////////////////////////////////////
}
