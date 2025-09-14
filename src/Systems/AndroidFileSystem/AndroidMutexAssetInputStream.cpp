#include "AndroidMutexAssetInputStream.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/AndroidAssetServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/DocumentHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // cppcheck-suppress uninitMemberVar
    AndroidMutexAssetInputStream::AndroidMutexAssetInputStream()
        : m_size( 0 )
        , m_offset( 0 )
        , m_carriage( 0 )
        , m_capacity( 0 )
        , m_reading( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidMutexAssetInputStream::~AndroidMutexAssetInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidMutexAssetInputStream::setAssetInputStream( const AndroidAssetInputStreamPtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidMutexAssetInputStream::setThreadMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _streaming );
        MENGINE_UNUSED( _share );

        size_t size = m_stream->size();

        if( _size != ~0U )
        {
            if( _offset + _size > size )
            {
                LOGGER_ERROR( "invalid relation '%s' folder '%s' file '%s' range %zu:%zu size %zu"
                    , _relationPath.c_str()
                    , _folderPath.c_str()
                    , _filePath.c_str()
                    , _offset
                    , _size
                    , (size_t)size
                );

                this->close();

                return false;
            }

            m_size = _size;
        }
        else
        {
            m_size = size;
        }

        m_offset = _offset;

        m_carriage = 0;
        m_capacity = 0;
        m_reading = 0;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::close()
    {
        m_stream = nullptr;
        m_mutex = nullptr;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidMutexAssetInputStream::read( void * const _buf, size_t _count )
    {
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
    bool AndroidMutexAssetInputStream::read_( void * const _buf, size_t _offset, size_t _size, size_t * const _read )
    {
        if( _size == 0 )
        {
            *_read = 0;

            return true;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        AAsset * asset = m_stream->getAsset();

        size_t current = m_reading - m_capacity + m_carriage;

        size_t pos = m_offset + current;

        int64_t result = ANDROID_ASSET_SERVICE()
            ->seek( asset, pos, SEEK_SET );

        if( result <= 0 )
        {
            LOGGER_ERROR( "error seek pos: %zu size: %zu"
                , pos
                , m_size
            );

            return false;
        }

        uint8_t * buf_offset = MENGINE_PVOID_OFFSET( _buf, _offset );

        size_t bytesRead = ANDROID_ASSET_SERVICE()
            ->read( asset, buf_offset, _size );

        *_read = bytesRead;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::seek( size_t _pos )
    {
        size_t carriage = _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidMutexAssetInputStream::rewind()
    {
        this->seek_( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::rseek( size_t _pos )
    {
        size_t carriage = m_size - _pos;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::skip( size_t _size )
    {
        size_t current = m_reading - m_capacity + m_carriage;

        size_t carriage = current + _size;

        bool successful = this->seek_( carriage );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::seek_( size_t _pos )
    {
        if( _pos >= m_reading - m_capacity && _pos <= m_reading )
        {
            m_carriage = m_capacity - (m_reading - _pos);
        }
        else
        {
            m_carriage = 0;
            m_capacity = 0;

            m_reading = _pos;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidMutexAssetInputStream::tell() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidMutexAssetInputStream::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::eof() const
    {
        size_t carriage = m_reading - m_capacity + m_carriage;

        return carriage == m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::time( uint64_t * const _time ) const
    {
        MENGINE_UNUSED( _time );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidMutexAssetInputStream::memory( void ** const _memory, size_t * const _size )
    {
        MENGINE_UNUSED( _memory );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
