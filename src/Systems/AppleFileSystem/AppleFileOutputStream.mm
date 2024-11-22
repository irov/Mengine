#include "AppleFileOutputStream.h"

#include "Interface/PlatformServiceInterface.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FilePathHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFileOutputStream::AppleFileOutputStream()
        : m_fileHandle( nil )
        , m_size( 0 )
        , m_withTemp( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileOutputStream::~AppleFileOutputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp )
    {
        m_relationPath = _relationPath;
        m_folderPath = _folderPath;
        m_filePath = _filePath;

        m_withTemp = _withTemp;

        Path concatenatePath = {'\0'};

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
        if( m_withTemp == true )
        {
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath, STRINGIZE_FILEPATH_LOCAL( ".~tmp" )}, concatenatePath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s' [temp]"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }
        else
        {
            if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, concatenatePath ) == false )
            {
                LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                    , m_folderPath.c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }
#else
        if( Helper::concatenateFilePath( {m_relationPath, m_folderPath, m_filePath}, concatenatePath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , m_folderPath.c_str()
                , m_filePath.c_str()
            );

            return false;
        }
#endif

        NSFileHandle * fileHandle = [NSFileHandle fileHandleForWritingAtPath:@(concatenatePath)];
        
        if( fileHandle == nil )
        {
            [[NSFileManager defaultManager] createFileAtPath:@(concatenatePath) contents:nil attributes:nil];
            
            fileHandle = [NSFileHandle fileHandleForWritingAtPath:@(concatenatePath)];
        }

        if( fileHandle == nil )
        {
            LOGGER_ERROR( "file %s invalid open"
                , concatenatePath
            );

            return false;
        }

        m_fileHandle = fileHandle;

        m_size = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::close()
    {
        if( m_fileHandle == nullptr )
        {
            return true;
        }

        [m_fileHandle closeFile];
        m_fileHandle = nil;

        m_size = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileOutputStream::write( const void * _data, size_t _size )
    {
        NSData * data = [NSData dataWithBytes:_data length:_size];
        
        NSError * error = nil;
        if( [m_fileHandle writeData:data error:&error] == NO )
        {
            LOGGER_ERROR( "invalid write file '%s:%s' size %zu [error: %s]"
                , m_folderPath.c_str()
                , m_filePath.c_str()
                , _size
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );

            return 0;
        }

        m_size += _size;

        return _size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileOutputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileOutputStream::flush()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    NSFileHandle * AppleFileOutputStream::getFileHandle() const
    {
        return m_fileHandle;
    }
    //////////////////////////////////////////////////////////////////////////
}
