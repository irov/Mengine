#include "Window.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct WindowQuad
    {
        mt::vec2f a;
        mt::vec2f b;
        mt::vec2f c;
        mt::vec2f d;
    };
    //////////////////////////////////////////////////////////////////////////
    const bool c_WindowWrapU[ResourceWindow_Count] =
    {
        true //ResourceWindow_Background
        , false //ResourceWindow_LeftTop
        , true //ResourceWindow_Top
        , false //ResourceWindow_RightTop
        , false //ResourceWindow_Right
        , false //ResourceWindow_RightBottom
        , true //ResourceWindow_Bottom
        , false //ResourceWindow_LeftBottom
        , false //ResourceWindow_Left
    };
    //////////////////////////////////////////////////////////////////////////
    const bool c_WindowWrapV[ResourceWindow_Count] =
    {
        true //ResourceWindow_Background
        , false //ResourceWindow_LeftTop
        , false //ResourceWindow_Top
        , false //ResourceWindow_RightTop
        , true //ResourceWindow_Right
        , false //ResourceWindow_RightBottom
        , false //ResourceWindow_Bottom
        , false //ResourceWindow_LeftBottom
        , true //ResourceWindow_Left
    };
    //////////////////////////////////////////////////////////////////////////
    Window::Window()
        : m_clientSize( 100.f, 100.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Window::~Window()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Window::_compile()
    {
        if( Node::_compile() == false )
        {
            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceWindow, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceWindow->compile() == false )
        {
            LOGGER_ERROR( "window '%s' resource '%s' invalid compile"
                , this->getName().c_str()
                , m_resourceWindow->getName().c_str()
            );

            return false;
        }

        for( uint32_t i = 0; i < ResourceWindow_Count; ++i )
        {
            const ResourceImagePtr & image = m_resourceWindow->getElementResourceImage( i );

            if( image == nullptr )
            {
                continue;
            }

            WindowEdge & edge = m_edge[i];

            const mt::vec2f & size = image->getSize();
            edge.initialSize = size;

            const RenderTextureInterfacePtr & texture = image->getTexture();
            const RenderTextureInterfacePtr & textureAlpha = image->getTextureAlpha();

            uint32_t textureCount = 0;

            mt::uv4f uvs[2];
            uvs[0] = image->getUVTextureImage();
            uvs[1] = image->getUVTextureAlpha();

            RenderTextureInterfacePtr textures[2];
            textures[0] = texture;
            textures[1] = textureAlpha;

            EMaterial materialId = EM_DEBUG;

            bool wrapU = c_WindowWrapU[i];
            bool wrapV = c_WindowWrapV[i];

            if( textureAlpha != nullptr )
            {
                if( wrapU == false && wrapV == false )
                {
                    materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA;
                }
                else if( wrapU == true && wrapV == false )
                {
                    materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WC;
                }
                else if( wrapU == true && wrapV == true )
                {
                    materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WW;
                }
                else if( wrapU == false && wrapV == true )
                {
                    materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA_CW;
                }

                textureCount = 2;
            }
            else if( texture != nullptr )
            {
                if( wrapU == false && wrapV == false )
                {
                    materialId = EM_TEXTURE_BLEND;
                }
                else if( wrapU == true && wrapV == false )
                {
                    materialId = EM_TEXTURE_BLEND_WC;
                }
                else if( wrapU == true && wrapV == true )
                {
                    materialId = EM_TEXTURE_BLEND_WW;
                }
                else if( wrapU == false && wrapV == true )
                {
                    materialId = EM_TEXTURE_BLEND_CW;
                }

                textureCount = 1;
            }
            else
            {
                materialId = EM_COLOR_BLEND;

                textureCount = 0;
            }

            RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( materialId, PT_TRIANGLELIST, textures, textureCount, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( material, "window '%s' resource '%s' invalid get material"
                , this->getName().c_str()
                , m_resourceWindow->getName().c_str()
            );

            edge.material = material;
        }

        this->invalidateVertices();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_release()
    {
        for( uint32_t i = 0; i != ResourceWindow_Count; ++i )
        {
            WindowEdge & edge = m_edge[i];

            edge.material = nullptr;
        }

        m_resourceWindow->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVertices();

        const mt::box2f * bb = this->getBoundingBox();

        if( this->hasBackground() == true )
        {
            const WindowEdge & edge = m_edge[ResourceWindow_Background];

            _renderPipeline->addRenderQuad( _context, edge.material, &vertices[0 * 4], 4, bb, false, MENGINE_DOCUMENT_FORWARD );
        }

        for( uint32_t i = 1; i != ResourceWindow_Count; ++i )
        {
            const WindowEdge & edge = m_edge[i];

            _renderPipeline->addRenderQuad( _context, edge.material, &vertices[i * 4], 4, bb, false, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_updateVertices( RenderVertex2D * const _vertices, uint8_t _invalidateVertices ) const
    {
        MENGINE_UNUSED( _invalidateVertices );

        WindowQuad quads[ResourceWindow_Count];

        const mt::vec2f & offsetLeftTop = m_resourceWindow->getElementOffset( ResourceWindow_LeftTop );

        quads[ResourceWindow_LeftTop].a.x = offsetLeftTop.x;
        quads[ResourceWindow_LeftTop].a.y = offsetLeftTop.y;

        quads[ResourceWindow_LeftTop].b.x = quads[ResourceWindow_LeftTop].a.x + m_edge[ResourceWindow_LeftTop].initialSize.x;
        quads[ResourceWindow_LeftTop].b.y = quads[ResourceWindow_LeftTop].a.y;

        quads[ResourceWindow_LeftTop].c.x = quads[ResourceWindow_LeftTop].b.x;
        quads[ResourceWindow_LeftTop].c.y = quads[ResourceWindow_LeftTop].b.y + m_edge[ResourceWindow_LeftTop].initialSize.y;

        quads[ResourceWindow_LeftTop].d.x = quads[ResourceWindow_LeftTop].a.x;
        quads[ResourceWindow_LeftTop].d.y = quads[ResourceWindow_LeftTop].c.y;

        /////////////////////////////////////////////////////////////////
        const mt::vec2f & offsetTop = m_resourceWindow->getElementOffset( ResourceWindow_Top );

        quads[ResourceWindow_Top].a.x = quads[ResourceWindow_LeftTop].b.x;
        quads[ResourceWindow_Top].a.y = offsetTop.y;

        const mt::vec2f & offsetRightTop = m_resourceWindow->getElementOffset( ResourceWindow_RightTop );
        float distance = m_clientSize.x - ((m_edge[ResourceWindow_LeftTop].initialSize.x + offsetLeftTop.x) + (m_edge[ResourceWindow_RightTop].initialSize.x - offsetRightTop.x));

        quads[ResourceWindow_Top].b.x = quads[ResourceWindow_Top].a.x + distance;
        quads[ResourceWindow_Top].b.y = quads[ResourceWindow_Top].a.y;

        quads[ResourceWindow_Top].c.x = quads[ResourceWindow_Top].b.x;
        quads[ResourceWindow_Top].c.y = quads[ResourceWindow_Top].a.y + m_edge[ResourceWindow_Top].initialSize.y;

        quads[ResourceWindow_Top].d.x = quads[ResourceWindow_Top].a.x;
        quads[ResourceWindow_Top].d.y = quads[ResourceWindow_Top].c.y;

        /////////////////////////////////////////////////////////////////

        quads[ResourceWindow_RightTop].a.x = quads[ResourceWindow_Top].b.x;
        quads[ResourceWindow_RightTop].a.y = offsetRightTop.y;

        quads[ResourceWindow_RightTop].b.x = quads[ResourceWindow_RightTop].a.x + m_edge[ResourceWindow_RightTop].initialSize.x;
        quads[ResourceWindow_RightTop].b.y = quads[ResourceWindow_RightTop].a.y;

        quads[ResourceWindow_RightTop].c.x = quads[ResourceWindow_RightTop].b.x;
        quads[ResourceWindow_RightTop].c.y = quads[ResourceWindow_RightTop].b.y + m_edge[ResourceWindow_RightTop].initialSize.y;

        quads[ResourceWindow_RightTop].d.x = quads[ResourceWindow_RightTop].a.x;
        quads[ResourceWindow_RightTop].d.y = quads[ResourceWindow_RightTop].c.y;

        /////////////////////////////////////////////////////////////////
        const mt::vec2f & offsetRight = m_resourceWindow->getElementOffset( ResourceWindow_Right );

        quads[ResourceWindow_Right].b.x = m_clientSize.x + offsetRight.x;
        quads[ResourceWindow_Right].b.y = quads[ResourceWindow_RightTop].d.y;

        const mt::vec2f & offsetRightBottom = m_resourceWindow->getElementOffset( ResourceWindow_RightBottom );
        float distanceRight = m_clientSize.y - ((m_edge[ResourceWindow_RightTop].initialSize.y + offsetRightTop.y) + (m_edge[ResourceWindow_RightBottom].initialSize.y - offsetRightBottom.y));

        quads[ResourceWindow_Right].c.x = quads[ResourceWindow_Right].b.x;
        quads[ResourceWindow_Right].c.y = quads[ResourceWindow_Right].b.y + distanceRight;

        quads[ResourceWindow_Right].d.x = quads[ResourceWindow_Right].b.x - m_edge[ResourceWindow_Right].initialSize.x;
        quads[ResourceWindow_Right].d.y = quads[ResourceWindow_Right].c.y;

        quads[ResourceWindow_Right].a.x = quads[ResourceWindow_Right].d.x;
        quads[ResourceWindow_Right].a.y = quads[ResourceWindow_Right].b.y;

        /////////////////////////////////////////////////////////////////

        quads[ResourceWindow_RightBottom].b.x = m_clientSize.x + offsetRightBottom.x;
        quads[ResourceWindow_RightBottom].b.y = quads[ResourceWindow_Right].c.y;

        quads[ResourceWindow_RightBottom].c.x = quads[ResourceWindow_RightBottom].b.x;
        quads[ResourceWindow_RightBottom].c.y = quads[ResourceWindow_RightBottom].b.y + m_edge[ResourceWindow_RightBottom].initialSize.y;

        quads[ResourceWindow_RightBottom].d.x = quads[ResourceWindow_RightBottom].c.x - m_edge[ResourceWindow_RightBottom].initialSize.x;
        quads[ResourceWindow_RightBottom].d.y = quads[ResourceWindow_RightBottom].c.y;

        quads[ResourceWindow_RightBottom].a.x = quads[ResourceWindow_RightBottom].d.x;
        quads[ResourceWindow_RightBottom].a.y = quads[ResourceWindow_RightBottom].b.y;

        /////////////////////////////////////////////////////////////////

        const mt::vec2f & offsetBottom = m_resourceWindow->getElementOffset( ResourceWindow_Bottom );

        quads[ResourceWindow_Bottom].c.x = quads[ResourceWindow_RightBottom].a.x;
        quads[ResourceWindow_Bottom].c.y = m_clientSize.y + offsetBottom.y;

        const mt::vec2f & offsetLeftBottom = m_resourceWindow->getElementOffset( ResourceWindow_LeftBottom );
        float distanceBottom = m_clientSize.x - ((m_edge[ResourceWindow_LeftBottom].initialSize.x + offsetLeftBottom.x) + (m_edge[ResourceWindow_RightBottom].initialSize.x - offsetRightBottom.x));

        quads[ResourceWindow_Bottom].d.x = quads[ResourceWindow_Bottom].c.x - distanceBottom;
        quads[ResourceWindow_Bottom].d.y = quads[ResourceWindow_Bottom].c.y;

        quads[ResourceWindow_Bottom].a.x = quads[ResourceWindow_Bottom].d.x;
        quads[ResourceWindow_Bottom].a.y = quads[ResourceWindow_Bottom].c.y - m_edge[ResourceWindow_Bottom].initialSize.y;

        quads[ResourceWindow_Bottom].b.x = quads[ResourceWindow_Bottom].c.x;
        quads[ResourceWindow_Bottom].b.y = quads[ResourceWindow_Bottom].a.y;

        /////////////////////////////////////////////////////////////////

        quads[ResourceWindow_LeftBottom].d.x = offsetLeftBottom.x;
        quads[ResourceWindow_LeftBottom].d.y = m_clientSize.y + offsetLeftBottom.y;

        quads[ResourceWindow_LeftBottom].a.x = quads[ResourceWindow_LeftBottom].d.x;
        quads[ResourceWindow_LeftBottom].a.y = quads[ResourceWindow_LeftBottom].d.y - m_edge[ResourceWindow_LeftBottom].initialSize.y;

        quads[ResourceWindow_LeftBottom].b.x = quads[ResourceWindow_Bottom].a.x;
        quads[ResourceWindow_LeftBottom].b.y = quads[ResourceWindow_LeftBottom].a.y;

        quads[ResourceWindow_LeftBottom].c.x = quads[ResourceWindow_LeftBottom].b.x;
        quads[ResourceWindow_LeftBottom].c.y = quads[ResourceWindow_LeftBottom].d.y;

        /////////////////////////////////////////////////////////////////

        const mt::vec2f & offsetLeft = m_resourceWindow->getElementOffset( ResourceWindow_Left );

        quads[ResourceWindow_Left].d.x = offsetLeft.x;
        quads[ResourceWindow_Left].d.y = quads[ResourceWindow_LeftBottom].a.y;

        float distanceLeft = m_clientSize.y - ((m_edge[ResourceWindow_LeftTop].initialSize.y + offsetLeftTop.y) + (m_edge[ResourceWindow_LeftBottom].initialSize.y - offsetLeftBottom.y));

        quads[ResourceWindow_Left].a.x = quads[ResourceWindow_Left].d.x;
        quads[ResourceWindow_Left].a.y = quads[ResourceWindow_Left].d.y - distanceLeft;

        quads[ResourceWindow_Left].b.x = quads[ResourceWindow_Left].a.x + m_edge[ResourceWindow_Left].initialSize.x;
        quads[ResourceWindow_Left].b.y = quads[ResourceWindow_Left].a.y;

        quads[ResourceWindow_Left].c.x = quads[ResourceWindow_Left].b.x;
        quads[ResourceWindow_Left].c.y = quads[ResourceWindow_Left].d.y;

        /////////////////////////////////////////////////////////////////

        quads[ResourceWindow_Background].a = mt::vec2f( 0.f, 0.f );
        quads[ResourceWindow_Background].b = mt::vec2f( m_clientSize.x, 0.f );
        quads[ResourceWindow_Background].c = mt::vec2f( m_clientSize.x, m_clientSize.y );
        quads[ResourceWindow_Background].d = mt::vec2f( 0.f, m_clientSize.y );

        /////////////////////////////////////////////////////////////////

        mt::vec2f uvs[ResourceWindow_Count];

        if( this->hasBackground() == true )
        {
            uvs[0] = mt::vec2f( m_clientSize.x / m_edge[0].initialSize.x, m_clientSize.y / m_edge[0].initialSize.y );
        }
        else
        {
            uvs[0] = mt::vec2f( 1.f, 1.f );
        }

        uvs[1] = mt::vec2f( 1.f, 1.f );
        uvs[2] = mt::vec2f( m_clientSize.x / m_edge[2].initialSize.x, 1.f );
        uvs[3] = mt::vec2f( 1.f, 1.f );
        uvs[4] = mt::vec2f( 1.f, m_clientSize.y / m_edge[4].initialSize.y );
        uvs[5] = mt::vec2f( 1.f, 1.f );
        uvs[6] = mt::vec2f( m_clientSize.x / m_edge[6].initialSize.x, 1.f );
        uvs[7] = mt::vec2f( 1.f, 1.f );
        uvs[8] = mt::vec2f( 1.f, m_clientSize.y / m_edge[8].initialSize.y );

        const mt::mat4f & worldMatrix = this->getWorldMatrix();

        for( uint32_t i = 0; i != ResourceWindow_Count; ++i )
        {
            mt::uv4f uv1;
            mt::uv4f uv2;

            const ResourceImagePtr & image = m_resourceWindow->getElementResourceImage( i );

            if( image != nullptr )
            {
                const mt::uv4f & uv_image = image->getUVTextureImage();
                const mt::uv4f & uv_alpha = image->getUVTextureAlpha();

                mt::uv4_scale( &uv1, uv_image, uvs[i] );
                mt::uv4_scale( &uv2, uv_alpha, uvs[i] );
            }

            const WindowQuad & quad = quads[i];

            RenderVertex2D & v0 = _vertices[i * 4 + 0];
            mt::mul_v3_v3_m4( &v0.position, mt::vec3f( quad.a, 0.f ), worldMatrix );

            v0.uv[0] = uv1[0];
            v0.uv[1] = uv2[0];

            RenderVertex2D & v1 = _vertices[i * 4 + 1];
            mt::mul_v3_v3_m4( &v1.position, mt::vec3f( quad.b, 0.f ), worldMatrix );

            v1.uv[0] = uv1[1];
            v1.uv[1] = uv2[1];


            RenderVertex2D & v2 = _vertices[i * 4 + 2];
            mt::mul_v3_v3_m4( &v2.position, mt::vec3f( quad.c, 0.f ), worldMatrix );

            v2.uv[0] = uv1[2];
            v2.uv[1] = uv2[2];

            RenderVertex2D & v3 = _vertices[i * 4 + 3];
            mt::mul_v3_v3_m4( &v3.position, mt::vec3f( quad.d, 0.f ), worldMatrix );

            v3.uv[0] = uv1[3];
            v3.uv[1] = uv2[3];
        }

        Color color;
        this->calcTotalColor( &color );

        ColorValue_ARGB argb = color.getAsARGB();

        for( RenderVertex2D
            * it = _vertices,
            *it_end = _vertices + ResourceWindow_Count * 4;
            it != it_end;
            ++it )
        {
            RenderVertex2D & vtx = *it;

            vtx.color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVertices();

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t i = 1; i != 4; ++i )
        {
            mt::box2_add_internal_point( _boundingBox, vertices[i].position.x, vertices[i].position.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_invalidateWorldMatrix() const
    {
        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_invalidateColor() const
    {
        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::_dispose()
    {
        m_resourceWindow = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::setClientSizeClip( const mt::vec2f & _clientSize )
    {
        if( this->hasBackground() == false )
        {
            LOGGER_ERROR( "window '%s' background not exist"
                , this->getName().c_str()
            );

            return;
        }

        mt::vec2f clSize = _clientSize;

        const WindowEdge & background = m_edge[ResourceWindow_Background];

        if( background.initialSize.x > 0.001f )
        {
            clSize.x = StdMath::ceilf( clSize.x / background.initialSize.x ) * background.initialSize.x;
        }

        if( background.initialSize.y > 0.001f )
        {
            clSize.y = StdMath::ceilf( clSize.y / background.initialSize.y ) * background.initialSize.y;
        }

        this->setClientSize( clSize );
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::setClientSize( const mt::vec2f & _clientSize )
    {
        m_clientSize = _clientSize;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Window::getClientSize() const
    {
        return m_clientSize;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Window::getWindowSize() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "window '%s' not compiled"
                , this->getName().c_str()
            );

            return mt::vec2f( 0.f, 0.f );
        }

        mt::vec2f windowSize = m_clientSize;
        windowSize += m_edge[ResourceWindow_LeftTop].initialSize;
        windowSize += m_edge[ResourceWindow_RightBottom].initialSize;

        return windowSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f Window::getTileSize( uint32_t _tile ) const
    {
        if( _tile >= ResourceWindow_Count )
        {
            LOGGER_ERROR( "window '%s' invalid tile argument '%u'"
                , this->getName().c_str()
                , _tile
            );

            return mt::vec2f( 0.f, 0.f );
        }

        const mt::vec2f & size = m_edge[_tile].initialSize;

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::setClientSizeInTiles( const mt::vec2f & _tiles )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "window '%s' not compiled."
                , this->getName().c_str()
            );

            return;
        }

        if( this->hasBackground() == false )
        {
            LOGGER_ERROR( "window '%s' background not exist"
                , this->getName().c_str()
            );

            return;
        }

        this->setClientSize( mt::vec2f( static_cast<int32_t>(_tiles.x) * m_edge[ResourceWindow_Background].initialSize.x,
            static_cast<int32_t>(_tiles.y) * m_edge[ResourceWindow_Background].initialSize.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Window::setResourceWindow( const ResourceWindowPtr & _resourceWindow )
    {
        if( m_resourceWindow == _resourceWindow )
        {
            return;
        }

        this->recompile( [this, &_resourceWindow]()
        {
            m_resourceWindow = _resourceWindow;

            if( m_resourceWindow == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceWindowPtr & Window::getResourceWindow() const
    {
        return m_resourceWindow;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Window::hasBackground() const
    {
        if( m_edge[ResourceWindow_Background].material == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
