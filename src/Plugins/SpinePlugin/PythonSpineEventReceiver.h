#pragma once

#include "SpineInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"

namespace Mengine
{
    class PythonSpineEventReceiver
        : public PythonAnimatableEventReceiver<SamplerSpineEventReceiverInterface>
    {
    public:
        PythonSpineEventReceiver();
        ~PythonSpineEventReceiver() override;

    public:
        void onSamplerSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue ) override;
        void onSamplerSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd ) override;
    };
}
