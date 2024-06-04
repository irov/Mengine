#include "AndroidAssetInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/AndroidAssetServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/AssertionNotImplemented.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    AndroidAssetInputStream::AndroidAssetInputStream()
        : m_asset( nullptr )
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
    AndroidAssetInputStream::~AndroidAssetInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::open" );

#if defined(MENGINE_DEBUG)
        this->setDebugRelationPath( _relationPath );
        this->setDebugFolderPath( _folderPath );
        this->setDebugFilePath( _filePath );
#endif

        m_streaming = _streaming;
        m_share = _share;

        Char fullPath[MENGINE_MAX_PATH] = {'\0'};
        if( this->openFile_( _relationPath, _folderPath, _filePath, fullPath ) == false )
        {
            return false;
        }

        int64_t size = ANDROID_ASSET_SERVICE()
            ->size( m_asset );

        if( size < 0 )
        {
            LOGGER_ERROR( "invalid file '%s' error size %zu"
                , fullPath
                , (size_t)size
            );

            this->close();

            return false;
        }

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
            int64_t result = ANDROID_ASSET_SERVICE()
                    ->seek( m_asset, m_offset, SEEK_SET );

            if( result <= 0 )
            {
                LOGGER_ERROR( "invalid file '%s' seek offset %zu size %zu"
                    , fullPath
                    , m_offset
                    , m_size
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::openFile_( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * const _fullPath )
    {
        if( Helper::concatenateFilePath( {_relationPath, _folderPath, _filePath}, _fullPath ) == false )
        {
            LOGGER_ERROR( "invalid concatenate filePath '%s:%s:%s'"
                , _relationPath.c_str()
                , _folderPath.c_str()
                , _filePath.c_str()
            );

            return false;
        }

        AAsset * asset = ANDROID_ASSET_SERVICE()
            ->open( _fullPath, AASSET_MODE_UNKNOWN );

        if( asset == nullptr )
        {
            LOGGER_ERROR( "invalid open file '%s'"
                , _fullPath
            );

            return false;
        }

        m_asset = asset;

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_OPEN_FILE, folderPath.c_str(), filePath.c_str(), m_streaming );
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::close()
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::close" );

        if( m_asset == nullptr )
        {
            return true;
        }

#if defined(MENGINE_DEBUG)
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            const FilePath & folderPath = this->getDebugFolderPath();
            const FilePath & filePath = this->getDebugFilePath();

            NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_CLOSE_FILE, folderPath.c_str(), filePath.c_str(), m_streaming );
        }
#endif

        ANDROID_ASSET_SERVICE()
            ->close( m_asset );

        m_asset = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidAssetInputStream::read( void * const _buf, size_t _count )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::read" );

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
    bool AndroidAssetInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = ANDROID_ASSET_SERVICE()
            ->read( m_asset, buf_offset, _size );

        if( bytesRead == 0 )
        {
            *_read = 0;

            return true;
        }

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::seek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::seek" );

        bool successful = this->seek_( _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidAssetInputStream::rewind()
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::rewind" );

        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::rseek( size_t _pos )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::seek" );

        bool successful = this->seek_( m_size - _pos );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            int64_t seek_pos = m_offset + _pos;

            int64_t result = ANDROID_ASSET_SERVICE()
                ->seek( m_asset, seek_pos, SEEK_SET );

            if( result < 0 )
            {
                LOGGER_ERROR( "file '%s' error seek %zu:%zu"
                    , Helper::getDebugFullPath( this )
                    , _pos
                    , m_size
                );

                return false;
            }

            m_carriage = 0;
            m_capacity = 0;

            m_reading = static_cast<size_t>(result) - m_offset;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::skip( size_t _size )
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::skip" );

        size_t current = m_reading - m_capacity + m_carriage;

        size_t seek_pos = current + _size;

        bool result = this->seek_( seek_pos );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidAssetInputStream::tell() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::tell" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidAssetInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::eof() const
    {
        MENGINE_THREAD_GUARD_SCOPE( AndroidAssetInputStream, this, "AndroidAssetInputStream::eof" );

        size_t current = m_reading - m_capacity + m_carriage;

        return current == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidAssetInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    AAsset * AndroidAssetInputStream::getAsset()
    {
        return m_asset;
    }
    //////////////////////////////////////////////////////////////////////////
}
