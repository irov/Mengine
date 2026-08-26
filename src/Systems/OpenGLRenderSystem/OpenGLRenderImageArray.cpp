#include "OpenGLRenderImageArray.h"

#include "OpenGLRenderErrorHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageArray::OpenGLRenderImageArray()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageArray::~OpenGLRenderImageArray()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GLenum OpenGLRenderImageArray::getTextureTarget() const
    {
        return GL_TEXTURE_2D_ARRAY;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageArray::_create()
    {
#if defined(MENGINE_RENDER_OPENGL_NORMAL_LINUX)
        if( GLAD_GL_ARB_texture_storage == 0 )
        {
            LOGGER_ERROR( "OpenGL texture 2d array requires GL_ARB_texture_storage" );

            return false;
        }
#endif

        MENGINE_GLCALL( glBindTexture, (GL_TEXTURE_2D_ARRAY, m_uid) );

        MENGINE_IF_GLCALL( glTexStorage3D, (GL_TEXTURE_2D_ARRAY, m_hwMipmaps, m_internalFormat, m_hwWidth, m_hwHeight, m_hwLayers) )
        {
            MENGINE_GLCALL( glBindTexture, (GL_TEXTURE_2D_ARRAY, 0) );

            return false;
        }

        MENGINE_GLCALL( glBindTexture, (GL_TEXTURE_2D_ARRAY, 0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderImageArray::_unlock( const Rect & _lockedRect, const void * _buffer, uint32_t _layer, uint32_t _level )
    {
        bool successful = true;

        uint32_t miplevel_hwwidth = m_hwWidth >> _level;
        uint32_t miplevel_hwheight = m_hwHeight >> _level;

        uint32_t miplevel_xoffset = _lockedRect.left >> _level;
        uint32_t miplevel_yoffset = _lockedRect.top >> _level;
        uint32_t miplevel_width = (_lockedRect.right - _lockedRect.left) >> _level;
        uint32_t miplevel_height = (_lockedRect.bottom - _lockedRect.top) >> _level;

        switch( m_internalFormat )
        {
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
#if defined(MENGINE_RENDER_OPENGL_NORMAL_LINUX)
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
#endif
            {
                if( _lockedRect.full( m_hwWidth, m_hwHeight ) == false )
                {
                    return false;
                }

                GLsizei textureMemorySize = (GLsizei)Helper::getTextureMemorySize( miplevel_hwwidth, miplevel_hwheight, m_hwPixelFormat );

                MENGINE_IF_GLCALL( glCompressedTexSubImage3D, (GL_TEXTURE_2D_ARRAY, _level, 0, 0, _layer, miplevel_hwwidth, miplevel_hwheight, 1, m_internalFormat, textureMemorySize, _buffer) )
                {
                    successful = false;
                }
            }break;
        default:
            {
                MENGINE_IF_GLCALL( glTexSubImage3D, (GL_TEXTURE_2D_ARRAY, _level, miplevel_xoffset, miplevel_yoffset, _layer, miplevel_width, miplevel_height, 1, m_format, m_type, _buffer) )
                {
                    successful = false;
                }
            }break;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
