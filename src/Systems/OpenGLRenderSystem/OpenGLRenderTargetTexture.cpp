#include "OpenGLRenderTargetTexture.h"

#include "OpenGLRenderError.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderTargetTexture::OpenGLRenderTargetTexture()
        : m_uid( 0 )
        , m_width( 0 )
        , m_height( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwChannels( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_minFilter( GL_LINEAR )
        , m_magFilter( GL_LINEAR )
        , m_wrapS( GL_CLAMP_TO_EDGE )
        , m_wrapT( GL_CLAMP_TO_EDGE )
        , m_internalFormat( GL_RGB )
        , m_format( GL_RGB )
        , m_type( GL_UNSIGNED_BYTE )
        , m_pow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderTargetTexture::~OpenGLRenderTargetTexture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type )
    {
        MENGINE_ASSERTION_FATAL( _width != 0 );
        MENGINE_ASSERTION_FATAL( _height != 0 );

        switch( _internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            {
                return false;
            }break;
        default:
            break;
        }

        GLuint tuid = 0;
        GLCALL( glGenTextures, (1, &tuid) );

        if( tuid == 0 )
        {
            LOGGER_ERROR( "invalid gen texture for size %d:%d channel %d PF %d"
                , _width
                , _height
                , _channels
                , _format
            );

            return false;
        }

        m_uid = tuid;

        m_width = _width;
        m_height = _height;
        m_hwWidth = Helper::getTexturePOW2( _width );
        m_hwHeight = Helper::getTexturePOW2( _height );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_hwChannels = _channels;
        m_hwPixelFormat = _pixelFormat;
        m_internalFormat = _internalFormat;
        m_format = _format;
        m_type = _type;

        m_pow2 = Helper::isTexturePOW2( _width ) && Helper::isTexturePOW2( _height );

        return true;
    }    
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::finalize()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::release()
    {
        if( m_uid != 0 )
        {
            GLCALL( glDeleteTextures, (1, &m_uid) );

            m_uid = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::reload()
    {
        GLuint tuid = 0;
        GLCALL( glGenTextures, (1, &tuid) );

        m_uid = tuid;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWMipmaps() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWDepth() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat OpenGLRenderTargetTexture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenGLRenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::begin()
    {
        //IDirect3DSurface9 * pD3DSurface;
        //DXCALL( m_pD3DTexture, GetSurfaceLevel, (0, &pD3DSurface) );

        //if( pD3DSurface == nullptr )
        //{
        //    return false;
        //}

        //IDirect3DSurface9 * pD3DSurfaceOld;
        //DXCALL( m_pD3DDevice, GetRenderTarget, (0, &pD3DSurfaceOld) );

        //DXCALL( m_pD3DDevice, SetRenderTarget, (0, pD3DSurface) );

        //m_pD3DSurfaceOld = pD3DSurfaceOld;
        //m_pD3DSurface = pD3DSurface;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::end()
    {
        //DXCALL( m_pD3DDevice, SetRenderTarget, (0, m_pD3DSurfaceOld) );

        //if( m_pD3DSurfaceOld != nullptr )
        //{
        //    m_pD3DSurfaceOld->Release();
        //    m_pD3DSurfaceOld = nullptr;
        //}

        //m_pD3DSurface->Release();
        //m_pD3DSurface = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    GLuint OpenGLRenderTargetTexture::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::_destroy()
    {
        this->release();
    }
}
