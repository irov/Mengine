#   include "CURLHTTPRequestPerformerSync.h"
#   include "CURLHTTPRequestPerformerPostForm.h"
#   include "CURLHTTPRequestPerformerAsync.h"
#   include "CURLHTTPUtils.h"

namespace Menge
{ 
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerAsync::CURLHTTPRequestPerformerAsync( const HTTPRequest& _request )
        : CURLHTTPRequestPerformer( _request )
		, m_multiHandle( NULL )
		, m_performer( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerAsync::~CURLHTTPRequestPerformerAsync()
    {
		_clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformerAsync::_init(void)
	{
		if( m_request.requestType == HTTPM_POST )
		{
			m_performer = new CURLHTTPRequestPerformerPostForm( m_request );
		}
		else
		{
			m_performer = new CURLHTTPRequestPerformerSync( m_request );
		}			
		m_performer->initialise();
		
		m_easyHandle = m_performer->getEasyHandle();
		m_response = m_performer->getResponse();

		/* init a multi stack */
		m_multiHandle = curl_multi_init();
		
		/* add the individual transfers */
		curl_multi_add_handle(m_multiHandle, m_easyHandle);
	}
    //////////////////////////////////////////////////////////////////////////
    //you must call execute in some loop for perform async request
	void CURLHTTPRequestPerformerAsync::_execute()
    {
		//while(CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multiHandle, &still_running));
		//number of current transfers 				
		int stillRunningHandles;
		/* we start some action by calling perform right away */
		curl_multi_perform(m_multiHandle, &stillRunningHandles);			

		if( stillRunningHandles != 0)
		{
			return;
		}
				
		long responseCode;
		
		CURLcode curlCode = curl_easy_getinfo( m_easyHandle, CURLINFO_RESPONSE_CODE, &responseCode );

		if( curlCode != CURLE_OK  )
		{
			m_response.flags |= HTTPF_FAILED; 
		}
		else
		{
			m_response.flags |= HTTPF_SUCCES;
		}

		m_response.httpCode = responseCode;
		m_response.flags &= ~HTTPF_INPROGRESS;
		m_response.flags |= HTTPF_COMPLETE;		
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformerAsync::_clear()
	{
		if( m_multiHandle != NULL )
		{
			curl_multi_cleanup( m_multiHandle );
			m_multiHandle = NULL;
		}
		if( m_performer != NULL )
		{
			delete m_performer;
			m_performer = NULL;
		}
		//dont forget to set this for parent destructor
		m_easyHandle = NULL;
	}
}
