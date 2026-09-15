#include "ToolMosaicRenderer.h"

#include "glad/gl.h"

#include <vector>

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const char * MOSAIC_VERTEX_SHADER = R"(#version 330 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec4 a_color;

uniform mat4 u_projection;

out vec2 v_uv;
out vec4 v_color;

void main()
{
    v_uv = a_uv;
    v_color = a_color.bgra;
    gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
}
)";
        //////////////////////////////////////////////////////////////////////////
        static const char * MOSAIC_FRAGMENT_SHADER = R"(#version 330 core
in vec2 v_uv;
in vec4 v_color;

uniform sampler2D u_texture;

out vec4 o_color;

void main()
{
    o_color = v_color * texture(u_texture, v_uv);
}
)";
        //////////////////////////////////////////////////////////////////////////
        static uint32_t compileShader_( uint32_t _stage, const char * _source )
        {
            uint32_t shader = glCreateShader( _stage );
            glShaderSource( shader, 1, &_source, nullptr );
            glCompileShader( shader );

            GLint compiled = GL_FALSE;
            glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

            if( compiled == GL_FALSE )
            {
                glDeleteShader( shader );

                return 0;
            }

            return shader;
        }
        //////////////////////////////////////////////////////////////////////////
        static void applyBlend_( Mosaic::BlendMode _blend )
        {
            switch( _blend )
            {
            case Mosaic::BlendMode::Alpha:
                glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                break;
            case Mosaic::BlendMode::PremultipliedAlpha:
                glBlendFuncSeparate( GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                break;
            case Mosaic::BlendMode::Additive:
                glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE );
                break;
            case Mosaic::BlendMode::Multiply:
                glBlendFuncSeparate( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                break;
            case Mosaic::BlendMode::Opaque:
                glBlendFuncSeparate( GL_ONE, GL_ZERO, GL_ONE, GL_ZERO );
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicRenderer::ToolMosaicRenderer()
        : m_program( 0 )
        , m_vertexArray( 0 )
        , m_vertexBuffer( 0 )
        , m_indexBuffer( 0 )
        , m_projectionLocation( -1 )
        , m_textureLocation( -1 )
        , m_handleEnumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicRenderer::~ToolMosaicRenderer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicRenderer::initialize()
    {
        uint32_t vertexShader = Detail::compileShader_( GL_VERTEX_SHADER, Detail::MOSAIC_VERTEX_SHADER );

        if( vertexShader == 0 )
        {
            return false;
        }

        uint32_t fragmentShader = Detail::compileShader_( GL_FRAGMENT_SHADER, Detail::MOSAIC_FRAGMENT_SHADER );

        if( fragmentShader == 0 )
        {
            glDeleteShader( vertexShader );

            return false;
        }

        m_program = glCreateProgram();
        glAttachShader( m_program, vertexShader );
        glAttachShader( m_program, fragmentShader );
        glLinkProgram( m_program );

        glDeleteShader( vertexShader );
        glDeleteShader( fragmentShader );

        GLint linked = GL_FALSE;
        glGetProgramiv( m_program, GL_LINK_STATUS, &linked );

        if( linked == GL_FALSE )
        {
            glDeleteProgram( m_program );
            m_program = 0;

            return false;
        }

        m_projectionLocation = glGetUniformLocation( m_program, "u_projection" );
        m_textureLocation = glGetUniformLocation( m_program, "u_texture" );

        glGenVertexArrays( 1, &m_vertexArray );
        glGenBuffers( 1, &m_vertexBuffer );
        glGenBuffers( 1, &m_indexBuffer );

        glBindVertexArray( m_vertexArray );
        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer );

        GLsizei stride = (GLsizei)sizeof( Mosaic::RenderVertex );

        glEnableVertexAttribArray( 0 );
        glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof( Mosaic::RenderVertex, position ) );

        glEnableVertexAttribArray( 1 );
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)offsetof( Mosaic::RenderVertex, uv ) );

        glEnableVertexAttribArray( 2 );
        glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (const void *)offsetof( Mosaic::RenderVertex, color ) );

        glBindVertexArray( 0 );

        const uint8_t white[4] = {0xffU, 0xffU, 0xffU, 0xffU};
        Mosaic::TextureHandle blank = this->createTexture( 1, 1, Mosaic::ByteSpan( reinterpret_cast<const std::byte *>(white), 4 ) );

        if( blank == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicRenderer::finalize()
    {
        for( const MapTextures::value_type & value : m_textures )
        {
            GLuint name = value.second;
            glDeleteTextures( 1, &name );
        }

        m_textures.clear();

        if( m_vertexBuffer != 0 )
        {
            glDeleteBuffers( 1, &m_vertexBuffer );
            m_vertexBuffer = 0;
        }

        if( m_indexBuffer != 0 )
        {
            glDeleteBuffers( 1, &m_indexBuffer );
            m_indexBuffer = 0;
        }

        if( m_vertexArray != 0 )
        {
            glDeleteVertexArrays( 1, &m_vertexArray );
            m_vertexArray = 0;
        }

        if( m_program != 0 )
        {
            glDeleteProgram( m_program );
            m_program = 0;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle ToolMosaicRenderer::createTexture_( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _pixels, bool _mask )
    {
        if( _width == 0 || _height == 0 )
        {
            return 0;
        }

        GLuint name = 0;
        glGenTextures( 1, &name );
        glBindTexture( GL_TEXTURE_2D, name );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        GLint internalFormat = _mask == true ? GL_R8 : GL_RGBA8;
        GLenum format = _mask == true ? GL_RED : GL_RGBA;

        glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, (GLsizei)_width, (GLsizei)_height, 0, format, GL_UNSIGNED_BYTE, _pixels.empty() == true ? nullptr : _pixels.data() );

        if( _mask == true )
        {
            const GLint swizzle[4] = {GL_RED, GL_RED, GL_RED, GL_RED};
            glTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle );
        }

        glBindTexture( GL_TEXTURE_2D, 0 );

        Mosaic::TextureHandle handle = ++m_handleEnumerator;

        m_textures.emplace( handle, name );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle ToolMosaicRenderer::createTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels )
    {
        Mosaic::TextureHandle handle = this->createTexture_( _width, _height, _rgbaPixels, false );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::TextureHandle ToolMosaicRenderer::createMaskTexture( uint32_t _width, uint32_t _height, Mosaic::ByteSpan _maskPixels )
    {
        Mosaic::TextureHandle handle = this->createTexture_( _width, _height, _maskPixels, true );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicRenderer::updateTexture( Mosaic::TextureHandle _texture, uint32_t _width, uint32_t _height, Mosaic::ByteSpan _rgbaPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, 0, 0, _width, _height, _width * 4, _rgbaPixels, false );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicRenderer::updateTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _rgbaPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, _x, _y, _width, _height, _bytesPerRow, _rgbaPixels, false );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicRenderer::updateMaskTextureRegion( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _maskPixels )
    {
        bool successful = this->updateTextureRegion_( _texture, _x, _y, _width, _height, _bytesPerRow, _maskPixels, true );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ToolMosaicRenderer::updateTextureRegion_( Mosaic::TextureHandle _texture, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _bytesPerRow, Mosaic::ByteSpan _pixels, bool _mask )
    {
        MapTextures::const_iterator it_found = m_textures.find( _texture );

        if( it_found == m_textures.end() )
        {
            return false;
        }

        if( _width == 0 || _height == 0 || _pixels.empty() == true )
        {
            return false;
        }

        uint32_t pixelSize = _mask == true ? 1U : 4U;

        glBindTexture( GL_TEXTURE_2D, it_found->second );
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, (GLint)(_bytesPerRow / pixelSize) );

        GLenum format = _mask == true ? GL_RED : GL_RGBA;

        glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)_x, (GLint)_y, (GLsizei)_width, (GLsizei)_height, format, GL_UNSIGNED_BYTE, _pixels.data() );

        glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
        glBindTexture( GL_TEXTURE_2D, 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicRenderer::destroyTexture( Mosaic::TextureHandle _texture )
    {
        MapTextures::const_iterator it_found = m_textures.find( _texture );

        if( it_found == m_textures.end() )
        {
            return;
        }

        GLuint name = it_found->second;
        glDeleteTextures( 1, &name );

        m_textures.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ToolMosaicRenderer::getTextureName( Mosaic::TextureHandle _texture ) const
    {
        MapTextures::const_iterator it_found = m_textures.find( _texture );

        if( it_found == m_textures.end() )
        {
            return 0;
        }

        return it_found->second;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicRenderer::render( const Mosaic::FrameViewport & _viewport, const Mosaic::RenderMesh & _mesh )
    {
        if( m_program == 0 || _mesh.batches.empty() == true )
        {
            return;
        }

        float width = _viewport.bounds.width;
        float height = _viewport.bounds.height;

        if( width <= 0.f || height <= 0.f )
        {
            return;
        }

        float left = _viewport.bounds.x;
        float right = _viewport.bounds.x + width;
        float top = _viewport.bounds.y;
        float bottom = _viewport.bounds.y + height;

        const float projection[16] = {
            2.f / (right - left), 0.f, 0.f, 0.f,
            0.f, 2.f / (top - bottom), 0.f, 0.f,
            0.f, 0.f, -1.f, 0.f,
            (right + left) / (left - right), (top + bottom) / (bottom - top), 0.f, 1.f
        };

        glEnable( GL_BLEND );
        glDisable( GL_CULL_FACE );
        glDisable( GL_DEPTH_TEST );
        glEnable( GL_SCISSOR_TEST );

        glUseProgram( m_program );
        glUniformMatrix4fv( m_projectionLocation, 1, GL_FALSE, projection );
        glUniform1i( m_textureLocation, 0 );

        glBindVertexArray( m_vertexArray );

        glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
        glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr)(_mesh.vertices.size() * sizeof( Mosaic::RenderVertex )), _mesh.vertices.data(), GL_STREAM_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(_mesh.indices.size() * sizeof( uint32_t )), _mesh.indices.data(), GL_STREAM_DRAW );

        glActiveTexture( GL_TEXTURE0 );

        GLsizei framebufferHeight = (GLsizei)height;

        for( const Mosaic::RenderBatch & batch : _mesh.batches )
        {
            if( batch.indexCount == 0 )
            {
                continue;
            }

            Mosaic::TextureHandle texture = 0;
            Mosaic::SamplerFilter sampler = Mosaic::SamplerFilter::Linear;
            Mosaic::BlendMode blend = Mosaic::BlendMode::PremultipliedAlpha;
            Mosaic::Rect clip = _viewport.bounds;

            if( batch.renderKey != 0 && batch.renderKey <= _mesh.renderStates.size() )
            {
                const Mosaic::RenderState & state = _mesh.renderStates[batch.renderKey - 1];

                texture = state.texture;
                sampler = state.sampler;
                blend = state.blend;

                if( state.clip.width > 0.f && state.clip.height > 0.f )
                {
                    clip = state.clip;
                }
            }

            Detail::applyBlend_( blend );

            GLint scissorX = (GLint)clip.x;
            GLint scissorY = (GLint)((float)framebufferHeight - (clip.y + clip.height));
            GLsizei scissorWidth = (GLsizei)clip.width;
            GLsizei scissorHeight = (GLsizei)clip.height;

            if( scissorWidth <= 0 || scissorHeight <= 0 )
            {
                continue;
            }

            glScissor( scissorX, scissorY, scissorWidth, scissorHeight );

            uint32_t name = this->getTextureName( texture );

            if( name == 0 )
            {
                name = this->getTextureName( 1 );
            }

            glBindTexture( GL_TEXTURE_2D, name );

            GLint filter = sampler == Mosaic::SamplerFilter::Nearest ? GL_NEAREST : GL_LINEAR;
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );

            const void * indexOffset = (const void *)(uintptr_t)(batch.indexOffset * sizeof( uint32_t ));

            glDrawElementsBaseVertex( GL_TRIANGLES, (GLsizei)batch.indexCount, GL_UNSIGNED_INT, indexOffset, (GLint)batch.vertexOffset );
        }

        glBindVertexArray( 0 );
        glUseProgram( 0 );
        glDisable( GL_SCISSOR_TEST );
    }
    //////////////////////////////////////////////////////////////////////////
}
