#pragma once

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Affector.h"
#include "Kernel/Scriptable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PythonValueFollower
        : public Affector        
        , public Scriptable
        , public PythonCallbackProvider
    {
    public:
        PythonValueFollower();
        ~PythonValueFollower() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PythonValueFollower, Affector> PythonValueFollowerPtr;
    //////////////////////////////////////////////////////////////////////////
}