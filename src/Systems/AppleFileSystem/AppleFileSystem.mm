#include "AppleFileSystem.h"

#include "Interface/PlatformServiceInterface.h"

#include "AppleFileGroupDirectory.h"
#include "AppleFileGroupDirectoryFactory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>

#include <sys/stat.h>
#include <dlfcn.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::AppleFileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool createDirectoryFullpath( const Char * _fullpath )
        {
            NSError * error = nil;
            if( [[NSFileManager defaultManager] createDirectoryAtPath:@(_fullpath)
                                          withIntermediateDirectories:YES
                                                           attributes:nil
                                                                error:&error] == NO )
            {
                LOGGER_ERROR( "invalid create directory: %s error: %s"
                    , _fullpath
                    , [error.description UTF8String]
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isDirectoryFullpath( const Char * _fullpath )
        {
            BOOL isDirectory = NO;
            if( [[NSFileManager defaultManager] fileExistsAtPath:@(_fullpath) isDirectory:&isDirectory] == NO )
            {
                return false;
            }

            return isDirectory;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileSystem::AppleFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileSystem::~AppleFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<AppleFileGroupDirectoryFactory, AppleFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Char utf8_currentPath[MENGINE_MAX_PATH + 1] = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );
        
        StdString::strcat( utf8_currentPath, "Data/" );
        
        size_t utf8_currentPathLen = StdString::strlen( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, (FilePath::size_type)utf8_currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<AppleFileGroupDirectoryFactory, AppleFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::existDirectory( const Char * _basePath, const Char * _directory ) const
    {
        Char pathDirectory[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        size_t len = StdString::strlen( pathDirectory );

        if( len == 0 )    // current dir
        {
            return true;
        }

        Char pathFull[MENGINE_MAX_PATH + 1] = {'\0'};
        StdString::strncpy( pathFull, _basePath, MENGINE_MAX_PATH );
        StdString::strncat( pathFull, pathDirectory, MENGINE_MAX_PATH );

        if( pathFull[len - 1] == ':' )    // root dir
        {
            return true;    // let it be
        }

        bool exist = Detail::isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::createDirectory( const Char * _basePath, const Char * _directory )
    {
        size_t directorySize = StdString::strlen( _directory );

        if( directorySize == 0 )
        {
            return true;
        }

        Char pathDirectory[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        if( StdString::strlen( pathDirectory ) == 0 )
        {
            return true;
        }

        Char pathTestDirectory[MENGINE_MAX_PATH + 1] = {'\0'};
        StdString::strncpy( pathTestDirectory, _basePath, MENGINE_MAX_PATH );
        StdString::strncat( pathTestDirectory, pathDirectory, MENGINE_MAX_PATH );

        if( Detail::isDirectoryFullpath(pathTestDirectory) == true )
        {
            return true;
        }

        Helper::pathRemoveBackslashA( pathDirectory );

        uint32_t paths_count = 0;
        Char paths[MENGINE_MAX_PATH + 1][16];

        for( ;; )
        {
            StdString::strncpy( paths[paths_count++], pathDirectory, MENGINE_MAX_PATH );

            if( Helper::pathRemoveFileSpecA( pathDirectory ) == false )
            {
                break;
            }

            if( StdString::strlen( _basePath ) == 0 )
            {
                break;
            }

            Helper::pathRemoveBackslashA( pathDirectory );

            StdString::strncpy( pathTestDirectory, _basePath, MENGINE_MAX_PATH );
            StdString::strncat( pathTestDirectory, pathDirectory, MENGINE_MAX_PATH );

            if( Detail::isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( uint32_t index = paths_count; index != 0; --index )
        {
            const Char * path = paths[index - 1];

            Char pathCreateDirectory[MENGINE_MAX_PATH + 1] = {'\0'};
            StdString::strncpy( pathCreateDirectory, _basePath, MENGINE_MAX_PATH );
            StdString::strncat( pathCreateDirectory, path, MENGINE_MAX_PATH );

            if( Detail::createDirectoryFullpath(pathCreateDirectory) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::existFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );
        
        if( [[NSFileManager defaultManager] fileExistsAtPath:@(pathCorrect)] == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::removeFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        NSError * error = nil;
        if( [[NSFileManager defaultManager] removeItemAtPath:@(pathCorrect) error:&error] == NO )
        {
            LOGGER_ERROR("Failed to remove file: %s error: %s"
                , pathCorrect
                , [error.localizedDescription UTF8String]
            );
            
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        Char oldPathCorrect[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Char newPathCorrect[MENGINE_MAX_PATH + 1] = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

        NSFileManager * fileManager = [NSFileManager defaultManager];
        
        if( [fileManager fileExistsAtPath:@(newPathCorrect)] == YES)
        {
            NSError * remove_error = nil;
            if( [fileManager removeItemAtPath:@(newPathCorrect) error:&remove_error] == NO )
            {
                LOGGER_ERROR("Failed to remove existing file: %s error: %s"
                    , newPathCorrect
                    , [remove_error.localizedDescription UTF8String]
                );
                
                return false;
            }
        }
        
        NSError * move_error = nil;
        if( [fileManager moveItemAtPath:@(oldPathCorrect) toPath:@(newPathCorrect) error:&move_error] == NO )
        {
            LOGGER_ERROR("Failed to move file from: %s to: %s error: %s"
                , oldPathCorrect
                , newPathCorrect
                , [move_error.localizedDescription UTF8String]
            );
            
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _base );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

        LOGGER_WARNING("SDLPlatformService::findFiles not support");

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t AppleFileSystem::getFileTime( const Char * _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
}
