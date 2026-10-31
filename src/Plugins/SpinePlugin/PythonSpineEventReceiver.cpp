#include "PythonSpineEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonSpineEventReceiver::PythonSpineEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonSpineEventReceiver::~PythonSpineEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSpineEventReceiver::onSamplerSpineEvent( const Char * _eventName, int32_t _eventIntValue, float _eventFloatValue, const Char * _eventStringValue )
    {
        m_cb.call( _eventName, _eventIntValue, _eventFloatValue, _eventStringValue );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonSpineEventReceiver::onSamplerSpineStateAnimationEnd( const ConstString & _state, const ConstString & _animation, bool _isEnd )
    {
        m_cb.call( _state, _animation, _isEnd );
    }
    //////////////////////////////////////////////////////////////////////////
}
