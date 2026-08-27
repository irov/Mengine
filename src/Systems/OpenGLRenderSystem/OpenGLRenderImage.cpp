#include "OpenGLRenderImage.h"

#include "OpenGLRenderErrorHelper.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::OpenGLRenderImage()
        : m_lockFirst( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImage::~OpenGLRenderImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImage::getTextureTarget() const
    {
        return GL_TEXTURE_2D;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::_create()
    {
        m_lockFirst = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImage::_unlock( const Rect & _lockedRect, const void * _buffer, uint32_t _layer, uint32_t _level )
    {
        MENGINE_UNUSED( _layer );

        bool successful = true;

        uint32_t miplevel_hwwidth = m_hwWidth >> _level;
        uint32_t miplevel_hwheight = m_hwHeight >> _level;

        switch( m_internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#if defined(MENGINE_RENDER_OPENGL_NORMAL_LINUX)
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
#endif
            {
                if( _lockedRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    GLuint textureMemorySize = Helper::getTextureMemorySize( miplevel_hwwidth, miplevel_hwheight, m_hwPixelFormat );
#if defined(MENGINE_RENDER_OPENGL_ES)
                    MENGINE_IF_GLCALL( glCompressedTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, _buffer) )
#else
                    MENGINE_IF_GLCALL( glCompressedTexImage2D_, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, textureMemorySize, _buffer) )
#endif
                    {
                        LOGGER_ASSERTION( "glCompressedTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n PixelFormat %d\n size %d"
                            , _level
                            , miplevel_hwwidth
                            , miplevel_hwheight
                            , m_internalFormat
                            , m_hwPixelFormat
                            , textureMemorySize
                        );

                        successful = false;
                    }
                }
                else
                {
                    successful = false;
                }
            }break;
        default:
            {
                if( _lockedRect.full( m_hwWidth, m_hwHeight ) == true )
                {
                    MENGINE_IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, _buffer) )
                    {
                        LOGGER_ASSERTION( "glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                            , _level
                            , miplevel_hwwidth
                            , miplevel_hwheight
                            , m_internalFormat
                            , m_format
                            , m_type
                            , m_hwPixelFormat
                        );

                        successful = false;
                    }
                }
                else
                {
                    uint32_t miplevel_xoffset = _lockedRect.left >> _level;
                    uint32_t miplevel_yoffset = _lockedRect.top >> _level;
                    uint32_t miplevel_width = (_lockedRect.right - _lockedRect.left) >> _level;
                    uint32_t miplevel_height = (_lockedRect.bottom - _lockedRect.top) >> _level;

                    if( m_lockFirst == true )
                    {
                        MENGINE_IF_GLCALL( glTexImage2D, (GL_TEXTURE_2D, _level, m_internalFormat, miplevel_hwwidth, miplevel_hwheight, 0x00000000, m_format, m_type, nullptr) )
                        {
                            LOGGER_ASSERTION( "glTexImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                                , _level
                                , miplevel_hwwidth
                                , miplevel_hwheight
                                , m_internalFormat
                                , m_format
                                , m_type
                                , m_hwPixelFormat
                            );

                            successful = false;
                        }
                    }

                    MENGINE_IF_GLCALL( glTexSubImage2D, (GL_TEXTURE_2D, _level, miplevel_xoffset, miplevel_yoffset, miplevel_width, miplevel_height, m_format, m_type, _buffer) )
                    {
                        LOGGER_ASSERTION( "glTexSubImage2D error\n level %d\n width %d\n height %d\n InternalFormat %d\n Format %d\n Type %d\n PixelFormat %d"
                            , _level
                            , miplevel_width
                            , miplevel_height
                            , m_internalFormat
                            , m_format
                            , m_type
                            , m_hwPixelFormat
                        );

                        successful = false;
                    }
                }
            }break;
        }

        m_lockFirst = false;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
