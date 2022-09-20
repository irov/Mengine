#pragma once

#include "Kernel/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryDestroyListenerInterface
        : public Factorable
    {
    public:
        virtual void onFactoryDestroyObject( Factorable * _object ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryDestroyListenerInterface> FactoryDestroyListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithListener
        : public Factory
    {
    public:
        FactoryWithListener();
        ~FactoryWithListener() override;

    public:
        void setDestroyListener( const FactoryDestroyListenerInterfacePtr & _listener );

    public:
        void destroyObject( Factorable * _object ) override;

    protected:
        FactoryDestroyListenerInterfacePtr m_destroyListener;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithListener, FactoryInterface> FactoryWithListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}
