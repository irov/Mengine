#include "Win32HttpRequestPostMessage.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestPostMessage::Win32HttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestPostMessage::~Win32HttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestPostMessage::setPostProperties( const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestPostProperties & Win32HttpRequestPostMessage::getPostProperties() const
    {
        return m_properties;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestPostMessage::_onHttp()
    {
        bool successful = Helper::httpRequestPostMessage( this, m_response, m_properties );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
