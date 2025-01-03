#include "PythonScheduleTiming.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTiming::PythonScheduleTiming()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScheduleTiming::~PythonScheduleTiming()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonScheduleTiming::onSchedulerTiming( UniqueId _id, uint32_t _iterate, float _delay )
    {
        this->call_cb( _id, _iterate, _delay );
    }
    ////////////////////////////////////////////////////////////////////////////
}