#pragma once

#include "Interface/ServantInterface.h"

#include "AndroidEventationInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AndroidEventationHub
        : public ServantInterface
    {
    public:
        AndroidEventationHub();
        ~AndroidEventationHub() override;

    public:
        bool initialize();
        void finalize();

    public:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation );
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation );

    public:
        void invoke();

    protected:
        typedef Vector<AndroidEventationInterfacePtr> VectorEventations;
        VectorEventations m_eventations;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidEventationHub> AndroidEventationHubPtr;
    ////////////////////////////////////////////////////////////////////
}