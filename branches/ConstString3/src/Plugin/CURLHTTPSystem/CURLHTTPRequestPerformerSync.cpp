#   include "CURLHTTPRequestPerformerSync.h"
#   include "CURLHTTPUtils.h"
namespace Menge
{ 
    /*static size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        CURLHTTPRequestPerformerSync * perfomer = static_cast<CURLHTTPRequestPerformerSync*>(userdata);
        perfomer->writeToResponse( ptr, size, nmemb );
        return size * nmemb;
    }*/
    
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerSync::CURLHTTPRequestPerformerSync( const HTTPRequest& _request )
        : CURLHTTPRequestPerformer( _request )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerSync::~CURLHTTPRequestPerformerSync()
    {
		_clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformerSync::_init()
	{
		m_easyHandle = curl_easy_init();
		if( !m_easyHandle )
		{
			puts(" CURLHTTPRequestPerformer:: m_easyHandle can not initialise ");
			return;
		}

		HTTPStringType sendData;
		for( size_t i = 0; i < m_request.variables.size(); ++i )
		{
			const HTTPVariable * var = &( m_request.variables[i] );
			sendData += CURLHTTPUtils::urlencode(var->name) + "=" + CURLHTTPUtils::urlencode(var->value) + "&";
		}

		if( sendData.size() != 0 )
		{
			sendData = sendData.substr( 0 ,sendData.size() - 1 );	
		}

		HTTPStringType url = m_request.url;
		
		if( m_request.requestType == HTTPM_POST )
		{
			curl_easy_setopt(m_easyHandle, CURLOPT_POSTFIELDS, sendData.c_str() );
		}
		else if( m_request.requestType == HTTPM_GET )
		{
			url += "?" + sendData;
			
			if( url.size() > HTTP_MAX_URL_SIZE - 1 )
			{
				url = url.substr( 0, HTTP_MAX_URL_SIZE );
			}
		}
		
		curl_easy_setopt( m_easyHandle, CURLOPT_URL, url.c_str() );

		curl_easy_setopt( m_easyHandle, CURLOPT_WRITEFUNCTION, writeRequestPerformerResponse );
		curl_easy_setopt( m_easyHandle, CURLOPT_WRITEDATA, this );
	}
}
