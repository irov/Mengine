#include "PrefetchedRenderImageLoader.h"

#include "Interface/RenderImageInterface.h"
#include "Kernel/MemoryCopy.h"
#include "Kernel/PixelFormatHelper.h"
#include "Config/StdLimits.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetchedRenderImageLoader::PrefetchedRenderImageLoader()
        : m_desc{}
        , m_codecFlags( 0 )
        , m_rowBytes( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetchedRenderImageLoader::~PrefetchedRenderImageLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchedRenderImageLoader::initialize( const RenderImageDesc & _desc, const MemoryBufferInterfacePtr & _pixels, uint32_t _codecFlags )
    {
        m_desc = _desc;
        m_pixels = _pixels;
        m_codecFlags = _codecFlags;

        uint32_t channels = Helper::getPixelFormatChannels( m_desc.format );
        m_rowBytes = size_t( m_desc.width ) * channels;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchedRenderImageLoader::getImageDesc( RenderImageDesc * const _desc ) const
    {
        *_desc = m_desc;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr PrefetchedRenderImageLoader::getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_UNUSED( _doc );

        if( _codecFlags != m_codecFlags )
        {
            return nullptr;
        }

        return m_pixels;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchedRenderImageLoader::load( const RenderImageInterfacePtr & _image ) const
    {
        uint32_t width = _image->getHWWidth();
        uint32_t height = _image->getHWHeight();
        EPixelFormat format = _image->getHWPixelFormat();

        if( m_desc.width == 0 || m_desc.height == 0 )
        {
            return false;
        }

        if( m_pixels == nullptr )
        {
            return false;
        }

        if( m_desc.width > width || m_desc.height > height )
        {
            return false;
        }

        if( m_desc.format != format )
        {
            return false;
        }

        uint32_t channels = Helper::getPixelFormatChannels( format );
        size_t maxSize = StdLimits::numeric_limits<size_t>::max();

        if( width > maxSize / channels )
        {
            return false;
        }

        size_t rowBytes = size_t( width ) * channels;
        Rect rect( 0, 0, width, height );
        RenderImageLockedInterfacePtr locked = _image->lock( 0, 0, rect, false );

        if( locked == nullptr )
        {
            return false;
        }

        size_t pitch = 0;
        uint8_t * destination = static_cast<uint8_t *>(locked->getLockedBuffer( &pitch ));

        if( destination == nullptr )
        {
            _image->unlock( locked, 0, 0, false );

            return false;
        }

        if( pitch < rowBytes || pitch > maxSize / height )
        {
            _image->unlock( locked, 0, 0, false );

            return false;
        }

        size_t bufferSize = pitch * height;
        StdString::memset( destination, 0, bufferSize );

        const uint8_t * source = m_pixels->getBuffer();

        for( uint32_t y = 0; y != m_desc.height; ++y )
        {
            size_t destinationOffset = y * pitch;
            size_t sourceOffset = y * m_rowBytes;
            Helper::memoryCopy( destination, destinationOffset, source, sourceOffset, m_rowBytes );

            if( m_desc.width < width )
            {
                size_t destinationEdge = destinationOffset + m_rowBytes;
                size_t sourceEdge = sourceOffset + m_rowBytes - channels;
                Helper::memoryCopy( destination, destinationEdge, source, sourceEdge, channels );
            }
        }

        if( m_desc.height < height )
        {
            size_t edgeBytes = m_rowBytes;

            if( m_desc.width < width )
            {
                edgeBytes += channels;
            }

            size_t destinationOffset = m_desc.height * pitch;
            size_t sourceOffset = (m_desc.height - 1) * pitch;
            Helper::memoryCopy( destination, destinationOffset, destination, sourceOffset, edgeBytes );
        }

        bool successful = _image->unlock( locked, 0, 0, true );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
