#pragma once

#include "Interface/RenderInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderVertexBufferInterface.h"
#include "Interface/RenderIndexBufferInterface.h"
#include "Interface/RenderProgramVariableInterface.h"

#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/Vector.h"

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
        void setExtraRelationRender( RenderInterface * _relationRender ) override;
        void removeExtraRelationRender() override;
        MENGINE_INLINE BaseRender * getExtraRelationRender() const override;

    public:
        MENGINE_INLINE BaseRender * getTotalRelationRender() const override;

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
        void setRenderViewport( const RenderViewportInterfacePtr & _renderViewport ) override;
        const RenderViewportInterfacePtr & getRenderViewport() const override;

    public:
        void setRenderCamera( const RenderCameraInterfacePtr & _renderCamera ) override;
        const RenderCameraInterfacePtr & getRenderCamera() const override;

    public:
        void setRenderTransformation( const RenderTransformationInterfacePtr & _renderTransformation ) override;
        const RenderTransformationInterfacePtr & getRenderTransformation() const override;

    public:
        void setRenderScissor( const RenderScissorInterfacePtr & _renderScissor ) override;
        const RenderScissorInterfacePtr & getRenderScissor() const override;

    public:
        void setRenderTarget( const RenderTargetInterfacePtr & _renderTarget ) override;
        const RenderTargetInterfacePtr & getRenderTarget() const override;

    public:
        void mergeRenderContext( const RenderContext * _in, RenderContext * const _out ) const override;
        void makeRenderContext( RenderContext * const _renderContext ) const override;

    public:
        void setRenderOrder( const RenderOrderInterfacePtr & _renderOrder ) override;
        const RenderOrderInterfacePtr & getRenderOrder() const override;

    public:
        void renderWithChildren( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external ) const override;

    public:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;

    public:
        void setExternalRender( bool _externalRender ) override;
        MENGINE_INLINE bool isExternalRender() const override;

    protected:
        virtual void _setExternalRender( bool _externalRender );

    protected:
        const Color & getWorldColor() const override;
        void calcTotalColor( Color * const _color ) const override;
        bool isSolidColor() const override;

    protected:
        void invalidateColor() const override;

    protected:
        void _setLocalTransparent( bool _transparent ) override;

    public:
        MENGINE_INLINE bool isRendering() const;

    protected:
        void updateRendering_() const;

    protected:
        void invalidateRendering();

    protected:
        BaseRender * m_relationRender;
        BaseRender * m_extraRelationRender;

        typedef Vector<BaseRender *> VectorBaseRender;
        VectorBaseRender m_renderChildren;

        RenderOrderInterfacePtr m_renderOrder;
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
    typedef IntrusivePtr<BaseRender> BaseRenderPtr;
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
    MENGINE_INLINE BaseRender * BaseRender::getExtraRelationRender() const
    {
        return m_extraRelationRender;
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
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE BaseRender * BaseRender::getTotalRelationRender() const
    {
        return m_extraRelationRender == nullptr ? m_relationRender : m_extraRelationRender;
    }
    //////////////////////////////////////////////////////////////////////////
}