#pragma once

#include "Interface/SemaphoreServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<SemaphoreListenerInterfacePtr> VectorSemaphoreListeners;
    //////////////////////////////////////////////////////////////////////////
    class Semaphore
        : public Factorable
    {
        DECLARE_FACTORABLE( Semaphore );

    public:
        Semaphore();
        ~Semaphore() override;

    public:
        void initalize( bool _activated );
        void finalize();

    public:
        bool isActivated() const;

    public:
        VectorSemaphoreListeners activate();

    public:
        void addListener( const SemaphoreListenerInterfacePtr & _listener );

    protected:
        bool m_activated;
        
        VectorSemaphoreListeners m_listeners;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Semaphore> SemaphorePtr;
    //////////////////////////////////////////////////////////////////////////
}
