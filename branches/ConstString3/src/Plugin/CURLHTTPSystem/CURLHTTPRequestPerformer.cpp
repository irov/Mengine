#   include "CURLHTTPRequestPerformer.h"

namespace Menge
{ 
	////////////////////////////////////////////////////////////////////////
	size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata)
	{
		CURLHTTPRequestPerformer * perfomer = static_cast<CURLHTTPRequestPerformer *>(userdata);
		perfomer->_writeToResponse( ptr, size, nmemb );
		return size * nmemb;
	}

	//////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformer::CURLHTTPRequestPerformer( const HTTPRequest& _request )
        : m_request(_request)
        , m_errorBuffer( NULL )
		, m_easyHandle( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformer::~CURLHTTPRequestPerformer()
    {
		_clear();	
    }
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPRequestPerformer::_writeToResponse( char *ptr, size_t size, size_t nmemb )
    {
        m_response.responseData.assign( ptr, size * nmemb );
    }
    //////////////////////////////////////////////////////////////////////////
    const HTTPResponse & CURLHTTPRequestPerformer::getResponse() const 
    {
        return m_response;
    }
	//////////////////////////////////////////////////////////////////////////
	CURL *  CURLHTTPRequestPerformer::getEasyHandle() const
	{
		return m_easyHandle;
	}
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPRequestPerformer::addReceiver( HTTPResponseReceiver * _receiver )
    {
        m_receivers.push_back( _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPRequestPerformer::setErrorBuffer( char * _buffer )
    {
        m_errorBuffer = _buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPRequestPerformer::execute()
    {
		if( (m_response.flags & HTTPF_INPROGRESS) == false )
		{
			initialise();
		}
		
		if( isComplete() == true )
		{
			return;
		}

		_execute();
		
		if( isComplete() == false )
		{
			return;
		}

		_sendToReceivers();
		_clear();
	}
    //////////////////////////////////////////////////////////////////////////
    void CURLHTTPRequestPerformer::_sendToReceivers()
    {
		for( THTTPReceiversVector::iterator 
			it = m_receivers.begin(), 
			it_end = m_receivers.end();
		it != it_end;
		it++ )
		{
			(*it)->receive(m_response);
		}

		//may be it`s not necessary
		m_receivers.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	bool CURLHTTPRequestPerformer::isComplete() const
	{
		if( (m_response.flags & HTTPF_COMPLETE) == false )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformer::_clear()
	{
		if( m_easyHandle != NULL )
		{
			curl_easy_cleanup( m_easyHandle );
			m_easyHandle = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformer::initialise()
	{
		m_response.flags = 0;
		m_response.flags |= HTTPF_INPROGRESS;
		_init();
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformer::_execute()
	{
		CURLcode curlCode;
		/* Perform the request, res will get the return code */
		curlCode = curl_easy_perform( m_easyHandle );

		m_response.flags |= HTTPF_COMPLETE;

		m_response.flags &= ~HTTPF_INPROGRESS;

		if( curlCode != CURLE_OK  )
		{
			m_response.flags |= HTTPF_FAILED; 
		}
		else
		{
			m_response.flags |= HTTPF_SUCCES;
		}
		
		long responseCode;
		curlCode = curl_easy_getinfo( m_easyHandle, CURLINFO_RESPONSE_CODE, &responseCode );

		if( curlCode != CURLE_OK  )
		{
			return;
		}
		
		m_response.httpCode = responseCode;
	}
	////////////////////////////////////////////////////////////////////////
}