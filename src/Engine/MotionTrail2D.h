#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Materialable.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Transformable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class MotionTrail2D
        : public Node
        , public Materialable
        , protected BaseRender
        , protected BaseTransformation
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( MotionTrail2D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();
        DECLARE_UPDATABLE();

    public:
        MotionTrail2D();
        ~MotionTrail2D() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;
        void setTarget( const TransformablePtr & _target );
        const TransformablePtr & getTarget() const;
        void setWidth( float _width );
        float getWidth() const;
        void setFadeTime( float _fadeTime );
        float getFadeTime() const;
        void setMinSegmentLength( float _length );
        float getMinSegmentLength() const;
        void setEmission( bool _emission );
        bool getEmission() const;
        void addPoint( const mt::vec2f & _position );
        void clearTrail();
        uint32_t getPointCount() const;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;
        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;
        RenderMaterialInterfacePtr _updateMaterial() const override;
        void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const override;

    protected:
        struct TrailPoint
        {
            mt::vec2f position;
            float age;
        };
        typedef Vector<TrailPoint> VectorTrailPoints;

        void appendPoint_( const mt::vec2f & _position );

    protected:
        ResourceImagePtr m_resourceImage;
        TransformablePtr m_target;
        VectorTrailPoints m_points;
        float m_width;
        float m_fadeTime;
        float m_minSegmentLength;
        bool m_emission;
    };

    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<MotionTrail2D> MotionTrail2DPtr;
}
