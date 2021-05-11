#include "MockupRenderImage.h"

#include "Interface/MemoryServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImage::MockupRenderImage()
        : m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwChannels( 0 )
        , m_hwDepth( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImage::~MockupRenderImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::initialize( uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, EPixelFormat _hwPixelFormat )
    {
        m_hwMipmaps = _mipmaps;
        m_hwWidth = _hwWidth;
        m_hwHeight = _hwHeight;
        m_hwChannels = _hwChannels;
        m_hwDepth = _hwDepth;
        m_hwPixelFormat = _hwPixelFormat;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::finalize()
    {
        m_memory = nullptr;

        m_renderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & MockupRenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer MockupRenderImage::lock( size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _readOnly );

        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        size_t size = (size_t)(_rect.right - _rect.left) * (_rect.bottom - _rect.top) * m_hwChannels * m_hwDepth;

        memory->newBuffer( size );

        m_memory = memory;

        *_pitch = (size_t)(_rect.right - _rect.left) * m_hwChannels * m_hwDepth;

        void * buffer = m_memory->getBuffer();

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderImage::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        m_memory = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MockupRenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWDepth() const
    {
        return 1; //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer MockupRenderImage::getRenderImageExtention()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
