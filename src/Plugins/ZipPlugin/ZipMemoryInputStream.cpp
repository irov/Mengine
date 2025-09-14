#include "ZipMemoryInputStream.h"

#include "Kernel/DebugFileHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ZipMemoryInputStream::ZipMemoryInputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ZipMemoryInputStream::~ZipMemoryInputStream()
    {
        this->close();
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipMemoryInputStream::setMemoryInputStream( const MemoryInputInterfacePtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInputInterfacePtr & ZipMemoryInputStream::getMemoryInputStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)        
        Helper::addDebugFilePath( this, _relationPath, _folderPath, _filePath, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::close()
    {
        m_stream = nullptr;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
        Helper::removeDebugFilePath( this );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer ZipMemoryInputStream::newBuffer( size_t _size )
    {
        return m_stream->newBuffer( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer ZipMemoryInputStream::getBuffer() const
    {
        return m_stream->getBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ZipMemoryInputStream::getSize() const
    {
        return m_stream->getSize();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ZipMemoryInputStream::read( void * const _buffer, size_t _size )
    {
        return m_stream->read( _buffer, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::seek( size_t _carriage )
    {
        return m_stream->seek( _carriage );
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipMemoryInputStream::rewind()
    {
        m_stream->rewind();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::rseek( size_t _carriage )
    {
        return m_stream->rseek( _carriage );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::skip( size_t _offset )
    {
        return m_stream->skip( _offset );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ZipMemoryInputStream::tell() const
    {
        return m_stream->tell();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ZipMemoryInputStream::size() const
    {
        return m_stream->size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::eof() const
    {
        return m_stream->eof();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::time( uint64_t * const _time ) const
    {
        return m_stream->time( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipMemoryInputStream::memory( void ** const _memory, size_t * const _size )
    {
        return m_stream->memory( _memory, _size );
    }
    //////////////////////////////////////////////////////////////////////////
}