#include "PythonNodeAreaOfInterestTriggerEventReceiver.h"

#include "NodeAreaOfInterestActor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonNodeAreaOfInterestTriggerEventReceiver::PythonNodeAreaOfInterestTriggerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonNodeAreaOfInterestTriggerEventReceiver::~PythonNodeAreaOfInterestTriggerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonNodeAreaOfInterestTriggerEventReceiver::onAreaOfInterestTriggerEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 )
    {
        NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

        m_cb.call( enemy, _iff1, _iff2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonNodeAreaOfInterestTriggerEventReceiver::onAreaOfInterestTriggerLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 )
    {
        NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

        m_cb.call( enemy, _iff1, _iff2 );
    }
    //////////////////////////////////////////////////////////////////////////
}
