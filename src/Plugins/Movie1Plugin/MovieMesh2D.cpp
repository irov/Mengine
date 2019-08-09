#include "MovieMesh2D.h" 

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "math/box2.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MovieMesh2D::MovieMesh2D()
        : m_shape( nullptr )
        , m_solid( false )
        , m_invalidateVerticesLocal( true )
        , m_invalidateVerticesWM( true )
        , m_invalidateVerticesColor( true )
        , m_vertexCount( 0 )
        , m_indicesCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieMesh2D::~MovieMesh2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieMesh2D::_compile()
    {
        if( this->compileResource_() == false )
        {
            return false;
        }

        this->invalidateMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieMesh2D::compileResource_()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, false, "'%s' image resource null"
            , this->getName().c_str()
        );

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "'%s' image resource '%s' not compile"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::_release()
    {
        m_resourceImage->release();

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, _resourceImage]()
        {
            m_resourceImage = _resourceImage;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & MovieMesh2D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr MovieMesh2D::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, m_solid, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( material, nullptr, "'%s' m_material is NULL"
            , this->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::render( const RenderContext * _context ) const
    {
        if( m_vertexCount == 0 )
        {
            return;
        }

        const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterialInterfacePtr & material = this->getMaterial();

        const mt::box2f * bb = this->getBoundingBox();

        this->addRenderObject( _context, material, nullptr, vertices, m_vertexCount, m_shape->indices, m_indicesCount, bb, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::_updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const
    {
        if( m_vertexCount == 0 )
        {
            *_boundingBoxCurrent = nullptr;

            return;
        }

        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t i = 1; i != m_vertexCount; ++i )
        {
            mt::add_internal_point( _boundingBox, vertices[i].position.x, vertices[i].position.y );
        }

        *_boundingBoxCurrent = &_boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::invalidateVertices()
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::invalidateVerticesWM()
    {
        m_invalidateVerticesWM = true;

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::updateVertices() const
    {
        m_invalidateVerticesLocal = false;

        for( uint32_t i = 0; i != m_vertexCount; ++i )
        {
            const mt::vec2f & uv = m_shape->uv[i];

            m_verticesWM[i].uv[0] = uv;
            m_verticesWM[i].uv[1] = uv;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::_invalidateColor()
    {
        this->invalidateVerticesColor();

        bool solid = this->isSolidColor();

        if( m_solid != solid )
        {
            m_solid = solid;

            this->invalidateMaterial();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::invalidateVerticesColor()
    {
        m_invalidateVerticesColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::updateVerticesColor() const
    {
        m_invalidateVerticesColor = false;

        Color color;
        this->calcTotalColor( color );

        const Color & textureColor = m_resourceImage->getColor();
        color *= textureColor;

        uint32_t argb = color.getAsARGB();

        for( RenderVertex2D
            * it = m_verticesWM,
            *it_end = m_verticesWM + m_vertexCount;
            it != it_end;
            ++it )
        {
            RenderVertex2D & vtx = *it;

            vtx.color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::setFrameShape( const MovieFrameShape * _shape )
    {
        m_shape = _shape;

        m_vertexCount = m_shape->vertexCount;
        m_indicesCount = m_shape->indexCount;

        this->invalidateVertices();
        this->invalidateVerticesColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieMesh2D::updateVerticesWM() const
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVerticesLocal == true )
        {
            this->updateVertices();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t i = 0; i != m_vertexCount; ++i )
        {
            const mt::vec2f & pos = m_shape->pos[i];

            mt::vec3f & wm_pos = m_verticesWM[i].position;
            mt::mul_v3_v2_m4( wm_pos, pos, wm );
        }
    }
}
