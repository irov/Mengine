#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Engine/Meshget.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonMeshEventReceiver
        : public PythonEventReceiver
        , public MeshgetEventReceiverInterface
        , public Factorable
    {
    public:
        PythonMeshEventReceiver();
        ~PythonMeshEventReceiver() override;

    public:
        void onMeshgetUpdate( const UpdateContext * _context ) override;
    };
}
