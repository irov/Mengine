#pragma once

#include "Interface/RenderCameraInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCamera
        : public Node
        , public Observable
        , public RenderCameraInterface
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        RenderCamera();
        ~RenderCamera() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    public:
        const mt::mat4f & getCameraViewMatrix() const override;
        const mt::mat4f & getCameraViewMatrixInv() const override;

        const mt::mat4f & getCameraProjectionMatrix() const override;
        const mt::mat4f & getCameraProjectionMatrixInv() const override;

    public:
        const mt::mat4f & getCameraViewProjectionMatrix() const override;
        const mt::mat4f & getCameraViewProjectionMatrixInv() const override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        void invalidateViewMatrix_() const;
        void invalidateProjectionMatrix_() const;

    protected:
        virtual void _updateViewMatrix() const = 0;
        virtual void _updateProjectionMatrix() const = 0;

    protected:
        void updateViewProjectionMatrix_() const;

    protected:
        void notifyChangeWindowResolution_( bool _fullscreen, const Resolution & _resolution );

    protected:
        mutable mt::mat4f m_viewMatrix;
        mutable mt::mat4f m_viewMatrixInv;

        mutable mt::mat4f m_projectionMatrix;
        mutable mt::mat4f m_projectionMatrixInv;

        mutable mt::mat4f m_viewProjectionMatrix;
        mutable mt::mat4f m_viewProjectionMatrixInv;

        mutable bool m_invalidateProjectionMatrix;
        mutable bool m_invalidateViewMatrix;
        mutable bool m_invalidateViewProjectionMatrix;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderCamera> RenderCameraPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraProjectionMatrix() const
    {
        if( m_invalidateProjectionMatrix == true )
        {
            m_invalidateProjectionMatrix = false;

            this->_updateProjectionMatrix();
        }

        return m_projectionMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraProjectionMatrixInv() const
    {
        if( m_invalidateProjectionMatrix == true )
        {
            m_invalidateProjectionMatrix = false;

            this->_updateProjectionMatrix();
        }

        return m_projectionMatrixInv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraViewMatrix() const
    {
        if( m_invalidateViewMatrix == true )
        {
            m_invalidateViewMatrix = false;

            this->_updateViewMatrix();
        }

        return m_viewMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraViewMatrixInv() const
    {
        if( m_invalidateViewMatrix == true )
        {
            m_invalidateViewMatrix = false;

            this->_updateViewMatrix();
        }

        return m_viewMatrixInv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraViewProjectionMatrix() const
    {
        if( m_invalidateViewProjectionMatrix == true )
        {
            m_invalidateViewProjectionMatrix = false;

            this->updateViewProjectionMatrix_();
        }

        return m_viewProjectionMatrix;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::mat4f & RenderCamera::getCameraViewProjectionMatrixInv() const
    {
        if( m_invalidateViewProjectionMatrix == true )
        {
            m_invalidateViewProjectionMatrix = false;

            this->updateViewProjectionMatrix_();
        }

        return m_viewProjectionMatrixInv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void RenderCamera::invalidateViewMatrix_() const
    {
        m_invalidateViewMatrix = true;
        m_invalidateViewProjectionMatrix = true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void RenderCamera::invalidateProjectionMatrix_() const
    {
        m_invalidateProjectionMatrix = true;
        m_invalidateViewProjectionMatrix = true;
    }
    //////////////////////////////////////////////////////////////////////////
}
