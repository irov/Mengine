#include "Win32HttpRequestGetMessage.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestGetMessage::Win32HttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestGetMessage::~Win32HttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestGetMessage::_onHttp()
    {
        bool successful = Helper::httpRequestGetMessage( this, m_response );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
