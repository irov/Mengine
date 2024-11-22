#include "AppleFileSystem.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"

#include "AppleFileGroupDirectory.h"
#include "AppleFileGroupDirectoryFactory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"
#include "Config/Path.h"

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
                    , [[AppleDetail getMessageFromNSError:error] UTF8String]
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

        Path utf8_currentPath = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );
        
        StdString::strncat( utf8_currentPath, "Data", MENGINE_MAX_PATH );
        StdString::strnchrcat( utf8_currentPath, MENGINE_PATH_DELIM_BACKSLASH, MENGINE_MAX_PATH );
        
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
        Path correctBasePath = {'\0'};
        Helper::pathCorrectBackslashToA( correctBasePath, _basePath );

        Path correctDirectory = {'\0'};
        Helper::pathCorrectBackslashToA( correctDirectory, _directory );

        size_t correctDirectoryLen = StdString::strlen( correctDirectory );

        if( correctDirectoryLen == 0 )
        {
            return true;
        }

        Path pathFull = {'\0'};
        Helper::pathCombineA( pathFull, correctBasePath, correctDirectory, MENGINE_PATH_DELIM_BACKSLASH );

        size_t pathFullLen = StdString::strlen( pathFull );
        
        if( pathFull[pathFullLen - 1] == ':' )
        {
            return true;
        }

        bool exist = Detail::isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::createDirectory( const Char * _basePath, const Char * _directory )
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
    bool AppleFileSystem::existFile( const Char * _filePath )
    {
        Path pathCorrect = {'\0'};
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
        Path pathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        NSError * error = nil;
        if( [[NSFileManager defaultManager] removeItemAtPath:@(pathCorrect) error:&error] == NO )
        {
            LOGGER_ERROR("Failed to remove file: %s error: %s"
                , pathCorrect
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );
            
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileSystem::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        Path oldPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Path newPathCorrect = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

        NSFileManager * fileManager = [NSFileManager defaultManager];
        
        if( [fileManager fileExistsAtPath:@(newPathCorrect)] == YES)
        {
            NSError * remove_error = nil;
            if( [fileManager removeItemAtPath:@(newPathCorrect) error:&remove_error] == NO )
            {
                LOGGER_ERROR("Failed to remove existing file: %s error: %s"
                    , newPathCorrect
                    , [[AppleDetail getMessageFromNSError:remove_error] UTF8String]
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
                , [[AppleDetail getMessageFromNSError:move_error] UTF8String]
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
        Path correctFilePath = {'\0'};
        Helper::pathCorrectBackslashToA( correctFilePath, _filePath );

        NSFileManager * fileManager = [NSFileManager defaultManager];
        
        NSDictionary<NSFileAttributeKey, id> * attributes = [fileManager attributesOfItemAtPath:@(correctFilePath) error:nil];

        if (attributes == nil) {
            return 0U;
        }
        
        NSDate * modificationDate = attributes[NSFileModificationDate];
        
        if (modificationDate == nil) {
            return 0U;
        }
        
        NSTimeInterval time = [modificationDate timeIntervalSince1970];
        
        return time;
    }
    //////////////////////////////////////////////////////////////////////////
}
