#	include "ThreadTaskGetMessage.h"

#	include "Interface/ConfigInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskGetMessage::ThreadTaskGetMessage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskGetMessage::initialize( const String & _url )
	{
		m_url = _url;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskGetMessage::_onCURL( CURL * _curl )
	{		
		curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

        this->setupWriteResponse( _curl );

        if( CONFIG_VALUE( m_serviceProvider, "HTTP", "Log", false ) == true )
        {
            LOGGER_STATISTIC( m_serviceProvider )("HTTP: get message url '%s'"
                , m_url.c_str()
                );
        }
	}
}
