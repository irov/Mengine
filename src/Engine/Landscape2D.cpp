#include "Landscape2D.h" 

#include "Interface/RenderCameraInterface.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "math/box2.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Landscape2D::Landscape2D()
        : m_elementCountX( 0 )
        , m_elementCountY( 0 )
        , m_elementWidth( 0.f )
        , m_elementHeight( 0.f )
        , m_invalidateVerticesWM( true )
        , m_invalidateElementWM( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Landscape2D::~Landscape2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Landscape2D::_compile()
    {
        if( m_images.empty() == true )
        {
            LOGGER_ERROR( "landscape '%s' images is empty"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_elementCountX == 0 || m_elementCountY == 0 )
        {
            LOGGER_ERROR( "landscape '%s' count X|Y not setup [%u:%u]"
                , this->getName().c_str()
                , m_elementCountX
                , m_elementCountY
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::_release()
    {
        for( Landscape2DElement & el : m_elements )
        {
            if( el.material != nullptr )
            {
                el.image->release();
                el.image = nullptr;

                el.material = nullptr;
            }
        }

        m_elements.clear();

        m_verticesWM.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::_dispose()
    {
        m_images.clear();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const VectorLandscape2DElements & elementsWM = this->getElementWM();

        mt::vec2f min_screen( 0.f, 0.f );
        mt::vec2f max_screen( 1.f, 1.f );

        const RenderCameraInterface * camera = _context->camera;

        const mt::mat4f & vpm = camera->getCameraViewProjectionMatrix();

        for( const Landscape2DElement & el : elementsWM )
        {
            mt::vec2f v_wvp_minimum;
            mt::mul_v2_v2_m4_homogenize( v_wvp_minimum, el.bb_wm.minimum, vpm );

            mt::vec2f v_wvp_minimum_norm;
            v_wvp_minimum_norm.x = (1.f + v_wvp_minimum.x) * 0.5f;
            v_wvp_minimum_norm.y = (1.f - v_wvp_minimum.y) * 0.5f;

            mt::vec2f v_wvp_maximum;
            mt::mul_v2_v2_m4_homogenize( v_wvp_maximum, el.bb_wm.maximum, vpm );

            mt::vec2f v_wvp_maximum_norm;
            v_wvp_maximum_norm.x = (1.f + v_wvp_maximum.x) * 0.5f;
            v_wvp_maximum_norm.y = (1.f - v_wvp_maximum.y) * 0.5f;

            if( mt::is_intersect( v_wvp_minimum_norm, v_wvp_maximum_norm, min_screen, max_screen ) == false )
            {
                if( el.material != nullptr )
                {
                    el.image->release();

                    el.material = nullptr;
                }
            }
            else
            {
                if( el.material == nullptr )
                {
                    if( el.image->compile() == false )
                    {
                        LOGGER_ERROR( "'%s' image resource '%s' not compile"
                            , this->getName().c_str()
                            , el.image->getName().c_str()
                        );

                        return;
                    }

                    RenderMaterialInterfacePtr material = this->makeImageMaterial( el.image, false, MENGINE_DOCUMENT_FACTORABLE );

                    MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' invalid get material"
                        , this->getName().c_str()
                    );

                    el.material = material;
                }
            }
        }

        uint32_t elementVertexOffset = 0;

        for( const Landscape2DElement & el : elementsWM )
        {
            if( el.material != nullptr )
            {
                const RenderVertex2D * vertices = this->getVerticesWM( elementVertexOffset );

                _renderPipeline->addRenderQuad( _context, el.material, vertices, 4, &el.bb_wm, false, MENGINE_DOCUMENT_FORWARD );
            }

            elementVertexOffset += 4;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::setBackParts( const VectorResourceImages & _images, uint32_t _countX, uint32_t _countY, float _width, float _height )
    {
        this->recompile( [this, &_images]()
        {
            m_images = _images;

            return true;
        } );

        m_elementCountX = _countX;
        m_elementCountY = _countY;
        m_elementWidth = _width;
        m_elementHeight = _height;

        uint32_t i = 0;
        uint32_t j = 0;

        VectorResourceImages::size_type elementCount = m_images.size();
        m_elements.reserve( elementCount );
        m_verticesWM.resize( elementCount * 4 );

        for( const ResourceImagePtr & image : m_images )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( image, "'%s' invalid setup image for [%u:%u]"
                , this->getName().c_str()
                , i
                , j
            );

            Landscape2DElement el;

            el.image = image;

            // left-upper
            mt::vec2f p1;
            p1.x = (i + 0) * m_elementWidth;
            p1.y = (j + 0) * m_elementHeight;

            mt::vec2f p2;
            p2.x = (i + 1) * m_elementWidth;
            p2.y = (j + 0) * m_elementHeight;

            mt::vec2f p3;
            p3.x = (i + 1) * m_elementWidth;
            p3.y = (j + 1) * m_elementHeight;

            mt::vec2f p4;
            p4.x = (i + 0) * m_elementWidth;
            p4.y = (j + 1) * m_elementHeight;

            mt::reset( el.bb, p1 );
            mt::add_internal_point( el.bb, p2 );
            mt::add_internal_point( el.bb, p3 );
            mt::add_internal_point( el.bb, p4 );

            el.i = i;
            el.j = j;

            ++i;

            if( i == m_elementCountX )
            {
                i = 0;
                ++j;
            }

            m_elements.emplace_back( el );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::updateVerticesWM_() const
    {
        m_invalidateVerticesWM = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        const uint32_t coeff_x[4] = { 0, 1, 1, 0 };
        const uint32_t coeff_y[4] = { 0, 0, 1, 1 };

        uint32_t vertex_offset = 0;

        for( const Landscape2DElement & el : m_elements )
        {
            const mt::uv4f & uv_image = el.image->getUVTextureImage();
            const mt::uv4f & uv_alpha = el.image->getUVTextureAlpha();

            for( uint32_t i = 0; i != 4; ++i )
            {
                // left-upper
                RenderVertex2D & v = m_verticesWM[vertex_offset + i];

                v.color = 0xffffffff;
                v.uv[0] = uv_image[i];
                v.uv[1] = uv_alpha[i];

                mt::vec3f p;
                p.x = (el.i + coeff_x[i]) * m_elementWidth;
                p.y = (el.j + coeff_y[i]) * m_elementHeight;
                p.z = 0.f;

                mt::mul_v3_v3_m4( v.position, p, wm );
            }

            vertex_offset += 4;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::updateElementWM_() const
    {
        m_invalidateElementWM = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        for( const Landscape2DElement & el : m_elements )
        {
            mt::mul_v2_v2_m4( el.bb_wm.minimum, el.bb.minimum, wm );
            mt::mul_v2_v2_m4( el.bb_wm.maximum, el.bb.maximum, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Landscape2D::_invalidateWorldMatrix() const
    {
        m_invalidateVerticesWM = true;
        m_invalidateElementWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Landscape2D::_updateMaterial() const
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}