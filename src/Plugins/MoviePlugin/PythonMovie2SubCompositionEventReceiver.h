#pragma once

#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Movie2SubCompositionEventReceiver.h"

namespace Mengine
{
    class PythonMovie2SubCompositionEventReceiver
        : public PythonAnimatableEventReceiver<Movie2SubCompositionEventReceiver>
    {
    public:
        PythonMovie2SubCompositionEventReceiver();
        ~PythonMovie2SubCompositionEventReceiver() override;
    };
}
