#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/PrefetcherObserverInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Unknowable.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontGlyphInterface
        : public ServantInterface
        , public Identity
        , public Observable
        , public Unknowable
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual void unfetch() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontGlyphInterface> FontGlyphInterfacePtr;
    //////////////////////////////////////////////////////////////////////////   
}
