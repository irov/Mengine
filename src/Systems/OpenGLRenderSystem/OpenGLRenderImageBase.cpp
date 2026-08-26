#include "OpenGLRenderImageBase.h"

#include "Interface/RenderSystemInterface.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderImageLockedFactoryStorage.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageBase::OpenGLRenderImageBase()
        : m_createTimestamp( 0 )
        , m_uid( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwLayers( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_pow2( false )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageBase::~OpenGLRenderImageBase()
    {
        MENGINE_ASSERTION_FATAL( m_uid == 0, "texture is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageBase::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _layers, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
        MENGINE_ASSERTION_FATAL( _width != 0, "invalid create texture width == 0" );
        MENGINE_ASSERTION_FATAL( _height != 0, "invalid create texture height == 0" );
        MENGINE_ASSERTION_FATAL( _layers != 0, "invalid create texture layers == 0" );

        switch( _internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            {
                if( _width != _height )
                {
                    LOGGER_ERROR( "not square texture %d:%d"
                        , _width
                        , _height
                    );

                    return false;
                }
            }break;
        default:
            break;
        }

        m_width = _width;
        m_height = _height;
        m_hwMipmaps = _mipmaps != 0 ? _mipmaps : 1;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );
        m_hwLayers = _layers;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_pow2 = Helper::isTexturePow2( _width ) == true && Helper::isTexturePow2( _height ) == true;
        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        if( this->create() == false )
        {
            LOGGER_ERROR( "invalid gen texture for size %u:%u PF %u"
                , _width
                , _height
                , _format
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImageBase::getHWLayerCount() const
    {
        return m_hwLayers;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::finalize()
    {
        this->release();

        m_renderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImageBase::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImageBase::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderImageBase::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat OpenGLRenderImageBase::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderImageBase::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderImageBase::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageBase::getUpscalePow2() const
    {
        return m_upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::bind( uint32_t _stage )
    {
#if defined(MENGINE_RENDER_OPENGL_ES)
        MENGINE_GLCALL( glActiveTexture, (GL_TEXTURE0 + _stage) );
#else
        MENGINE_GLCALL( glActiveTexture_, (GL_TEXTURE0 + _stage) );
#endif

        MENGINE_GLCALL( glBindTexture, (this->getTextureTarget(), m_uid) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::unbind( uint32_t _stage )
    {
#if defined(MENGINE_RENDER_OPENGL_ES)
        MENGINE_GLCALL( glActiveTexture, (GL_TEXTURE0 + _stage) );
#else
        MENGINE_GLCALL( glActiveTexture_, (GL_TEXTURE0 + _stage) );
#endif

        MENGINE_GLCALL( glBindTexture, (this->getTextureTarget(), 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & OpenGLRenderImageBase::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageBase::create()
    {
        MENGINE_ASSERTION_FATAL( m_uid == 0, "texture already created" );

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        GLuint tuid = extension->genTexture();

        if( tuid == 0 )
        {
            return false;
        }

        m_uid = tuid;

        if( this->_create() == false )
        {
            extension->deleteTexture( m_uid );
            m_uid = 0;

            return false;
        }

        int64_t textureMemorySize = (int64_t)m_hwWidth * (int64_t)m_hwHeight * (int64_t)m_hwLayers * (int64_t)Helper::getPixelFormatChannels( m_hwPixelFormat );

        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, textureMemorySize );

        LOGGER_DEBUG( "opengl", "render texture alloc add: %" MENGINE_PRId64 " total: %" MENGINE_PRId64
            , textureMemorySize
            , STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE )
        );

        m_createTimestamp = Helper::getSystemTimestamp();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::release()
    {
        if( m_uid == 0 )
        {
            return;
        }

        OpenGLRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        extension->deleteTexture( m_uid );

        m_uid = 0;

        int64_t textureMemorySize = (int64_t)m_hwWidth * (int64_t)m_hwHeight * (int64_t)m_hwLayers * (int64_t)Helper::getPixelFormatChannels( m_hwPixelFormat );

        STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, textureMemorySize );

        LOGGER_DEBUG( "opengl", "render texture alloc del: %" MENGINE_PRId64 " total: %" MENGINE_PRId64
            , textureMemorySize
            , STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE )
        );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr OpenGLRenderImageBase::lock( uint32_t _layer, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        if( _layer >= m_hwLayers || _level >= m_hwMipmaps )
        {
            return nullptr;
        }

        MENGINE_UNUSED( _readOnly );

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        if( miplevel_width == 0 || miplevel_height == 0 )
        {
            return nullptr;
        }

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwPixelFormat );

        OpenGLRenderImageLockedPtr imageLocked = OpenGLRenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        size_t pitch = size / miplevel_height;

        imageLocked->initialize( size, pitch, _rect );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_PIXEL, (_rect.bottom - _rect.top) * (_rect.right - _rect.left) );

        return imageLocked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageBase::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _layer, uint32_t _level, bool _successful )
    {
        if( _layer >= m_hwLayers || _level >= m_hwMipmaps )
        {
            return false;
        }

        if( _successful == false )
        {
            return true;
        }

        const Rect & lockedRect = _locked->getLockedRect();

        size_t pitch;
        const void * buffer = _locked->getLockedBuffer( &pitch );

        MENGINE_GLCALL( glBindTexture, (this->getTextureTarget(), m_uid) );

        bool successful = this->_unlock( lockedRect, buffer, _layer, _level );

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_UNLOCK_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_UNLOCK_PIXEL, (lockedRect.bottom - lockedRect.top) * (lockedRect.right - lockedRect.left) );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::setMinFilter( GLenum _minFilter )
    {
        m_minFilter = _minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImageBase::getMinFilter() const
    {
        return m_minFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::setMagFilter( GLenum _magFilter )
    {
        m_magFilter = _magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImageBase::getMagFilter() const
    {
        return m_magFilter;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::setWrapS( GLenum _wrapS )
    {
        m_wrapS = _wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImageBase::getWrapS() const
    {
        return m_wrapS;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::setWrapT( GLenum _wrapT )
    {
        m_wrapT = _wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImageBase::getWrapT() const
    {
        return m_wrapT;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderImageBase::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageBase::onRenderReset()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageBase::onRenderRestore()
    {
        if( this->create() == false )
        {
            return false;
        }

        if( m_renderImageProvider == nullptr )
        {
            return true;
        }

        RenderImageLoaderInterfacePtr loader = m_renderImageProvider->getLoader( MENGINE_DOCUMENT_FACTORABLE );

        if( loader->load( RenderImageInterfacePtr( this ) ) == false )
        {
            LOGGER_ERROR( "invalid decode image [%u]"
                , m_uid
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
