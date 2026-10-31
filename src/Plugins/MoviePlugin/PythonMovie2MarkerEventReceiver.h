#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Movie2MarkerEventReceiverInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonMovie2MarkerEventReceiver
        : public Factorable
        , public Movie2MarkerEventReceiverInterface
        , public PythonEventReceiver
    {
    public:
        PythonMovie2MarkerEventReceiver();
        ~PythonMovie2MarkerEventReceiver() override;

    public:
        void onMovieMarker( const ConstString & _name, float _time, uint32_t _iteration ) override;
    };
}
