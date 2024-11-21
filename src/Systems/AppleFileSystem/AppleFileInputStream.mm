#include "AppleFileInputStream.h"

#include "Interface/FileSystemInterface.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/StdString.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    AppleFileInputStream::AppleFileInputStream()
        : m_fileHandle( nil )
        , m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
        , m_streaming( false )
        , m_share( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileInputStream::~AppleFileInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

#if defined(MENGINE_DEBUG)
        this->setDebugRelationPath( _relationPath );
        this->setDebugFolderPath( _folderPath );
        this->setDebugFilePath( _filePath );
#endif

        m_streaming = _streaming;
        m_share = _share;

        Path fullPath = {'\0'};
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        NSError * error = nil;
        NSDictionary * attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:@(fullPath) error:&error];

        if( attributes == nil )
        {
            LOGGER_ERROR( "invalid file '%s' size [error: %s]"
                , fullPath
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );

            this->close();
            
            return false;
        }
        
        NSNumber * attribute_filesize = attributes[NSFileSize];
        
        size_t size = attribute_filesize.unsignedLongLongValue;
        
        if( _size != ~0U )
        {
            if( _offset + _size > (size_t)size )
            {
                LOGGER_ERROR( "invalid file '%s' range %zu:%zu size %zu"
                    , fullPath
                    , _offset
                    , _size
                    , (size_t)size
                );

                return false;
            }

            m_size = _size;
        }
        else
        {
            m_size = (size_t)size;
        }

        m_offset = _offset;

        m_carriage = 0;
        m_capacity = 0;
        m_reading = 0;

        if( m_offset != 0 )
        {
            NSError * error = nil;
            if( [m_fileHandle seekToOffset:m_offset error:&error] == NO )
            {
                LOGGER_ERROR( "file '%s' seek offset %zu size %zu get [error: %s]"
                    , fullPath
                    , m_offset
                    , m_size
                    , [[AppleDetail getMessageFromNSError:error] UTF8String]
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _fullPath )
    {
        if( Helper::concatenateFilePath( {_relationPath, _folderPath, _filePath}, _fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s'"
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        NSError * error = nil;
        NSFileHandle * fileHandle = [NSFileHandle fileHandleForReadingAtPath:@(_fullPath)];

        if( fileHandle == nil )
        {
            LOGGER_ERROR( "invalid open file '%s' error '%s'"
                , _fullPath
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );

            return false;
        }

        m_fileHandle = fileHandle;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, _folderPath, _filePath, true, m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::close()
{
        if( m_fileHandle == nullptr )
        {
            return true;
        }
        
#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();
            
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath, filePath, true, m_streaming );
        }
#endif
        
        NSError * error = nil;
        if( [m_fileHandle closeAndReturnError:&error] == NO )
        {
            LOGGER_ERROR( "invalid close file '%s' get error: %s"
                , Helper::getDebugFullPath( this ).c_str()
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );
            
            return false;
        }

        m_fileHandle = nil;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        size_t pos = m_reading - m_capacity + m_carriage;

        size_t correct_count = _count;

        if( pos + _count > m_size )
        {
            correct_count = m_size - pos;
        }

        if( correct_count > MENGINE_FILE_STREAM_BUFFER_SIZE )
        {
            size_t tail = m_capacity - m_carriage;

            if( tail != 0 )
            {
                stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
            }

            size_t toRead = correct_count - tail;

            size_t bytesRead;
            if( this->read_( _buf, tail, toRead, &bytesRead ) == false )
            {
                return 0;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading += bytesRead;

            return bytesRead + tail;
        }

        if( m_carriage + correct_count <= m_capacity )
        {
            stdex::memorycopy( _buf, 0, m_readCache + m_carriage, correct_count );

            m_carriage += correct_count;

            return correct_count;
        }

        size_t tail = m_capacity - m_carriage;

        if( tail != 0 )
        {
            stdex::memorycopy( _buf, 0, m_readCache + m_carriage, tail );
        }

        size_t bytesRead;
        if( this->read_( m_readCache, 0, MENGINE_FILE_STREAM_BUFFER_SIZE, &bytesRead ) == false )
        {
            return 0;
        }

        size_t readSize = MENGINE_MIN( correct_count - tail, bytesRead );

        stdex::memorycopy( _buf, tail, m_readCache, readSize );

        m_carriage = readSize;
        m_capacity = bytesRead;

        m_reading += bytesRead;

        return readSize + tail;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        NSError * error = nil;
        NSData * data = [m_fileHandle readDataUpToLength:_size error:&error];
        
        if( error != nil )
        {
            LOGGER_ERROR( "read file '%s' offset %zu size %zu:%zu get error %s"
                , Helper::getDebugFullPath( this ).c_str()
                , _offset
                , _size
                , m_size
                , [[AppleDetail getMessageFromNSError:error] UTF8String]
            );
            
            return false;
        }
        
        if( data.length == 0 )
        {
            *_read = 0;

            return true;
        }
        
        stdex::memorycopy( buf_offset, 0, data.bytes, data.length );

        *_read = data.length;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFileInputStream::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            size_t seek_pos = m_offset + _pos;

            NSError * error = nil;
            if( [m_fileHandle seekToOffset:seek_pos error:&error] == NO )
            {
                LOGGER_ERROR( "invalid file: %s seek: %zu size:%zu error: %s"
                    , Helper::getDebugFullPath( this ).c_str()
                    , seek_pos
                    , m_size
                    , [[AppleDetail getMessageFromNSError:error] UTF8String]
                );

                return false;
            }

            m_carriage = 0;
            m_capacity = 0;
            
            size_t result_seek = [m_fileHandle offsetInFile];

            m_reading = result_seek - m_offset;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _size;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AppleFileInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AppleFileInputStream, this );

        return (m_reading - m_capacity + m_carriage) == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::time( uint64_t * const _time ) const
    {
#if defined(MENGINE_DEBUG)
        const FilePath & relationPath = this->getDebugRelationPath();
        const FilePath & folderPath = this->getDebugFolderPath();
        const FilePath & filePath = this->getDebugFilePath();

        Path fullPath = {'\0'};
        if( Helper::concatenateFilePath( {relationPath, folderPath, filePath}, fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s:%s'"
                , relationPath.c_str()
                , folderPath.c_str()
                , filePath.c_str()
            );

            return false;
        }

        uint64_t ft = FILE_SYSTEM()
            ->getFileTime( fullPath );

        *_time = ft;

        return true;
#else
        MENGINE_UNUSED( _time );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFileInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    NSFileHandle * AppleFileInputStream::getFileHandle() const
    {
        return m_fileHandle;
    }
    //////////////////////////////////////////////////////////////////////////
}
