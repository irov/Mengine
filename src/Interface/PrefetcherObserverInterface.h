#pragma once

#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrefetcherObserverInterface
        : public Interface
    {
    public:
        virtual void onPrefetchIgnored() = 0;
        virtual void onPrefetchAlreadyExist() = 0;
        virtual void onPrefetchPreparation() = 0;
        virtual bool onPrefetchRun() = 0;
        virtual void onPrefetchCancel() = 0;
        virtual void onPrefetchComplete( bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrefetcherObserverInterface> PrefetcherObserverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}

