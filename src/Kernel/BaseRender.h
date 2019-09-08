#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderProgramVariableInterface.h"

#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseRender
        : public RenderInterface
    {
    public:
        BaseRender();
        ~BaseRender() override;

    public:
        void setRelationRender( RenderInterface * _relationRender ) override;
        void removeRelationRender() override;
        MENGINE_INLINE BaseRender * getRelationRender() const override;

    public:
        bool emptyRenderChildren() const override;
        void clearRenderChildren() override;

    public:
        void foreachRenderChildren( const LambdaRender & _lambda ) override;
        void foreachRenderChildrenEnabled( const LambdaRender & _lambda ) override;

    public:
        void setRenderEnable( bool _enable ) override;
        MENGINE_INLINE bool isRenderEnable() const override;

    public:
        void setHide( bool _hide ) override;
        MENGINE_INLINE bool isHide() const override;

    public:
        void setLocalHide( bool _localHide ) override;
        MENGINE_INLINE bool isLocalHide() const override;

    protected:
        virtual void _setHide( bool _hide );
        virtual void _setLocalHide( bool _localHide );

    protected:
        void addRelationRenderChildrenBack_( BaseRender * _childRender );
        void removeRelationRenderChildren_( BaseRender * _childRender );

    public:
        void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) override;
        const RenderViewportInterfacePtr & getRenderViewport() const override;

    public:
        void setRenderCamera( const RenderCameraInterfacePtr & _camera ) override;
        const RenderCameraInterfacePtr & getRenderCamera() const override;

    public:
        void setRenderTransformation( const RenderTransformationInterfacePtr & _renderTransformation ) override;
        const RenderTransformationInterfacePtr & getRenderTransformation() const override;

    public:
        void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) override;
        const RenderScissorInterfacePtr & getRenderScissor() const override;

    public:
        void setRenderTarget( const RenderTargetInterfacePtr & _target ) override;
        const RenderTargetInterfacePtr & getRenderTarget() const override;

    public:
        void renderWithChildren( const RenderContext * _context, bool _external ) const override;

    public:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;

    public:
        void setExternalRender( bool _externalRender ) override;
        MENGINE_INLINE bool isExternalRender() const override;

    protected:
        virtual void _setExternalRender( bool _externalRender );

    protected:
        void addRenderMesh( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _programVariable
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _vertexCount, uint32_t _indexCount ) const;

        void addRenderObject( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
            , const RenderProgramVariableInterfacePtr & _variable
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount
            , const mt::box2f * _bb, bool _debug ) const;

        void addRenderQuad( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug ) const;

        void addRenderLine( const RenderContext * _context, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug ) const;

    protected:
        const RenderMaterialInterfacePtr & getDebugMaterial() const;

    protected:
        const Color & getWorldColor() const override;
        void calcTotalColor( Color & _color ) const override;
        bool isSolidColor() const override;

    protected:
        void invalidateColor() override;

    protected:
        void _setLocalTransparent( bool _transparent ) override;

    public:
        MENGINE_INLINE bool isRendering() const;

    protected:
        void updateRendering_() const;

    protected:
        BaseRender * m_relationRender;

        typedef Vector<BaseRender *> VectorBaseRender;
        VectorBaseRender m_renderChildren;

        RenderViewportInterfacePtr m_renderViewport;
        RenderCameraInterfacePtr m_renderCamera;
        RenderTransformationInterfacePtr m_renderTransformation;
        RenderScissorInterfacePtr m_renderScissor;
        RenderTargetInterfacePtr m_renderTarget;

        bool m_externalRender;
        bool m_renderEnable;
        bool m_hide;
        bool m_localHide;

        mutable bool m_rendering;
        mutable bool m_invalidateRendering;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseRender::isRendering() const
    {
        if( m_invalidateRendering == true )
        {
            this->updateRendering_();
        }

        return m_rendering;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE BaseRender * BaseRender::getRelationRender() const
    {
        return m_relationRender;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseRender::isRenderEnable() const
    {
        return m_renderEnable;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseRender::isHide() const
    {
        return m_hide;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseRender::isLocalHide() const
    {
        return m_localHide;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool BaseRender::isExternalRender() const
    {
        return m_externalRender;
    }
}