#pragma once

#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Movie2EventReceiverInterface.h"

namespace Mengine
{
    class PythonMovie2EventReceiver
        : public PythonAnimatableEventReceiver<Movie2EventReceiverInterface>
    {
    public:
        PythonMovie2EventReceiver();
        ~PythonMovie2EventReceiver() override;
    };
}
