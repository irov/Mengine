#include "Win32HttpRequestDeleteMessage.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestDeleteMessage::Win32HttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestDeleteMessage::~Win32HttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestDeleteMessage::_onHttp()
    {
        bool successful = Helper::httpRequestDeleteMessage( this, m_response );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
