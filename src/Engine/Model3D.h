#pragma once

#include "Interface/Model3DInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Materialable.h"
#include "Kernel/BaseAnimation.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceHolder.h"

#include "ResourceModel3D.h"

#include "Kernel/ColourValue.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/RenderVertex2D.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderCameraProjection> RenderCameraProjectionPtr;
    //////////////////////////////////////////////////////////////////////////
    class Model3D
        : public Node
        , public Materialable
        , public BaseUpdation
        , public BaseAnimation
    {
    public:
        DECLARE_ANIMATABLE();

    public:
        Model3D();
        ~Model3D() override;

    public:
        void setResourceModel3D( const ResourceModel3DPtr & _resourceImage );
        const ResourceModel3DPtr & getResourceModel3D() const;

    protected:
        void _setTime( float _timming ) override;
        float _getTime() const override;

        void _setFirstFrame() override;
        void _setLastFrame() override;

    protected:
        bool _play( uint32_t _enumerator, float _time ) override;
        bool _restart( uint32_t _enumerator, float _time ) override;
        void _pause( uint32_t _enumerator ) override;
        void _resume( uint32_t _enumerator, float _time ) override;
        void _stop( uint32_t _enumerator ) override;
        void _end( uint32_t _enumerator ) override;
        bool _interrupt( uint32_t _enumerator ) override;

    protected:
        uint32_t getFrame_( float _time, float & _delthaTime ) const;
        void updateCurrentFrame_();
        void setCurrentFrame_( uint32_t _frame );

    protected:
        void _update( const UpdateContext * _context ) override;

    protected:
        void _render( const RenderContext * _state ) override;

        bool _activate() override;
        void _deactivate() override;

    protected:
        bool _compile() override;
        void _release() override;

        void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        bool compileResource_();

    protected:
        void invalidateVertices();
        void updateVertices() const;

    protected:
        void invalidateVerticesWM();
        void updateVerticesWM() const;

    protected:
        void invalidateVerticesColor();
        void updateVerticesColor() const;

    protected:
        inline const RenderVertex2D * getVerticesWM() const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ResourceHolder<ResourceModel3D> m_resourceModel;

        float m_frameTime;
        uint32_t m_currentFrame;

        RenderCameraProjectionPtr m_camera;

        const Model3DFrame * m_frame;

        mutable RenderVertex2D * m_verticesWM;

        uint32_t m_vertexCount;
        uint32_t m_indicesCount;

        bool m_solid;

        mutable bool m_invalidateVerticesLocal;
        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * Model3D::getVerticesWM() const
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
