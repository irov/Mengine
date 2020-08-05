#include "OpenGLRenderTargetTexture.h"

#include "OpenGLRenderExtension.h"
#include "OpenGLRenderError.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderTargetTexture::OpenGLRenderTargetTexture()
        : m_tuid( 0 )
        , m_fuid( 0 )
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

        if( this->create() == false )
        {
            return false;
        }

        return true;
    }    
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::finalize()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::create()
    {
        GLuint tuid = 0;
        GLCALL( glGenTextures, (1, &tuid) );

        GLCALL( glBindTexture, (GL_TEXTURE_2D, tuid) );
        GLCALL( glTexImage2D, (GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0) );

        m_tuid = tuid;

        GLuint fuid = 0;
        GLCALL( glGenFramebuffers, (1, &fuid) );

        if( fuid == 0 )
        {
            LOGGER_ERROR( "invalid gen framebuffer"
            );

            GLCALL( glDeleteTextures, (1, &m_tuid) );

            m_tuid = 0;

            return false;
        }

        m_fuid = fuid;

        GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, m_fuid) );

        GLCALL( glFramebufferTexture, (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tuid, 0) );

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        GLCALL( glDrawBuffers, (1, DrawBuffers) );

        if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
        {
            OPENGL_RENDER_CHECK_ERROR();

            GLCALL( glDeleteTextures, (1, &m_tuid) );

            m_tuid = 0;

            GLCALL( glDeleteFramebuffers, (1, &m_fuid) );

            m_fuid = 0;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::release()
    {
        if( m_tuid != 0 )
        {
            GLCALL( glDeleteTextures, (1, &m_tuid) );

            m_tuid = 0;
        }

        if( m_fuid != 0 )
        {
            GLCALL( glDeleteFramebuffers, (1, &m_fuid) );

            m_fuid = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderTargetTexture::reload()
    {
        if( this->create() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenGLRenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
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
        GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, m_fuid) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::end()
    {
        GLCALL( glBindFramebuffer, (GL_FRAMEBUFFER, 0) );
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
        return m_tuid;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderTargetTexture::_destroy()
    {
        this->release();
    }
}
