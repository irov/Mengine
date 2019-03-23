#pragma once

#include "Config/Lambda.h"

#include "Kernel/Affector.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/Factory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseAffector
        : public Affector
    {
    public:
        BaseAffector();
        ~BaseAffector() override;

    public:
        void setEasing( const EasingInterfacePtr & _easing );
        const EasingInterfacePtr & getEasing() const;

    public:
        void setCallback( const AffectorCallbackInterfacePtr & _cb );
        const AffectorCallbackInterfacePtr & getCallback() const;

    protected:
        void _complete( bool _isEnd ) override;

    protected:
        EasingInterfacePtr m_easing;
        AffectorCallbackInterfacePtr m_cb;
    };
}
