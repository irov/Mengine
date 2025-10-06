#pragma once

#include "Interface/RenderResolutionInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Observable.h"

#include "Kernel/Viewport.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderResolution
        : public Node
        , public Observable
        , public RenderResolutionInterface
    {
        DECLARE_FACTORABLE( RenderResolution );

    public:
        RenderResolution();
        ~RenderResolution() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    public:
        void setContentResolution( const Resolution & _resolution ) override;
        const Resolution & getContentResolution() const override;

    public:
        void fromContentToScreenPosition( const mt::vec2f & _contentPosition, mt::vec2f * const _screenPosition ) const override;
        void fromScreenToContentPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _contentPosition ) const override;

    protected:
        void notifyChangeWindowResolution_( bool _fullscreen, const Resolution & _resolution );

    protected:
        Resolution m_contentResolution;

        mt::vec2f m_contentResolutionSize;
        mt::vec2f m_contentResolutionSizeInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderResolution, RenderResolutionInterface> RenderResolutionPtr;
    //////////////////////////////////////////////////////////////////////////
}
