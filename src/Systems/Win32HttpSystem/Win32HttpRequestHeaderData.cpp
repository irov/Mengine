#include "Win32HttpRequestHeaderData.h"

#include "Win32HttpNetwork.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestHeaderData::Win32HttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequestHeaderData::~Win32HttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequestHeaderData::setData( const Data & _data )
    {
        m_data = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & Win32HttpRequestHeaderData::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequestHeaderData::_onHttp()
    {
        bool successful = Helper::httpRequestHeaderData( this, m_response, m_data );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
