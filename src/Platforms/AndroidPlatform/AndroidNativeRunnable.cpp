#include "AndroidNativeRunnable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeRunnable::AndroidNativeRunnable( const LambdaCallback & _callback )
        : m_callback( _callback )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeRunnable::~AndroidNativeRunnable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeRunnable::invoke()
    {
        LambdaCallback callback = m_callback;
        m_callback = nullptr;

        if( callback != nullptr )
        {
            callback();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeRunnable::release()
    {
        m_callback = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
