#pragma once

//#include "Interface/MovieKeyFrameInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"
#include "Kernel/Surface.h"
#include "Kernel/Color.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseUpdation.h"

#include "Config/Vector.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<mt::vec3f> VectorPositions;
    typedef Vector<mt::vec2f> VectorUVs;
    typedef Vector<mt::vec4f> VectorColors;
    //////////////////////////////////////////////////////////////////////////
    enum EMeshgetEventFlag
    {
        EVENT_MESHGET_UPDATE = 0,
        __EVENT_MESHGET_LAST__
    };
    //////////////////////////////////////////////////////////////////////////
    class MeshgetEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onMeshgetUpdate( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MeshgetEventReceiverInterface> MeshgetEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( MeshgetEventReceiverInterface, EVENT_MESHGET_UPDATE );
    //////////////////////////////////////////////////////////////////////////
    class Meshget
        : public Node
        , public BaseUpdation
        , public BaseRender
        , public BaseEventation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_EVENTABLE();

    public:
        Meshget();
        ~Meshget() override;

    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

    public:
        bool setVertices( const VectorPositions & _positions, const VectorUVs & _uv, const VectorColors & _colors, const VectorRenderIndex & _indices );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderContext * _context ) const override;

    protected:
        void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const override;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        void invalidateVerticesWM();
        void updateVerticesWM() const;

    protected:
        void invalidateVerticesColor();
        void updateVerticesColor() const;

    protected:
        MENGINE_INLINE const VectorRenderVertex2D & getVerticesWM() const;

    protected:
        SurfacePtr m_surface;

        VectorPositions m_positions;
        VectorUVs m_uvs;
        VectorColors m_colors;

        VectorRenderIndex m_indices;

        mutable VectorRenderVertex2D m_verticesWM;

        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const VectorRenderVertex2D & Meshget::getVerticesWM() const
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM();
        }

        if( m_invalidateVerticesColor == true )
        {
            this->updateVerticesColor();
        }

        return m_verticesWM;
    }
}
