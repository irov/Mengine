#include "Win32HttpRequestPing.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestPing::Win32HttpRequestPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestPing::~Win32HttpRequestPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestPing::_onHttp()
    {
        bool successful = Helper::httpRequestGetMessage( this, m_response );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
