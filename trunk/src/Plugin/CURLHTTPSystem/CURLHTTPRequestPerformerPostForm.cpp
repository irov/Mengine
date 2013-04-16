#   include "CURLHTTPRequestPerformerPostForm.h"

namespace Menge
{ 
    /*static size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        CURLHTTPRequestPerformerPostForm * perfomer = static_cast<CURLHTTPRequestPerformerPostForm*>(userdata);
        perfomer->writeToResponse( ptr, size, nmemb );
        return size * nmemb;
    }*/

    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerPostForm::CURLHTTPRequestPerformerPostForm( const HTTPRequest& _request )
        : CURLHTTPRequestPerformer( _request )
		, m_formpost(NULL)
		, m_headerlist(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CURLHTTPRequestPerformerPostForm::~CURLHTTPRequestPerformerPostForm()
    {
		_clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformerPostForm::_clear()
	{
		if(m_easyHandle != NULL )
		{
			curl_easy_cleanup(m_easyHandle);
			m_easyHandle = NULL;
		}
		if(m_formpost != NULL )
		{
			/* free slist */
			curl_formfree(m_formpost);
			m_formpost = NULL;
		}
		if(m_headerlist != NULL )
		{
			/* then cleanup the formpost chain */
			curl_slist_free_all(m_headerlist);
			m_headerlist = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void CURLHTTPRequestPerformerPostForm::_init()
	{
		struct curl_httppost * lastptr = NULL;
	
		static const char buf[] = "Expect:";
		m_easyHandle = curl_easy_init();
		
		if( !m_easyHandle )
		{
			puts("HTTPLoggerCURL:: m_easyHandle can not inititalise");
			return;
		}

        for( size_t i = 0; i < m_request.variables.size(); ++i )
		{
			/* Fill in the  field */
			curl_formadd(&m_formpost,
				&lastptr,
				CURLFORM_COPYNAME, m_request.variables[i].name.c_str(),
				CURLFORM_COPYCONTENTS, m_request.variables[i].value.c_str(),
				CURLFORM_END);
		}
		
		/* initalize custom header list (stating that Expect: 100-continue is not
			wanted */
		m_headerlist = curl_slist_append(m_headerlist, buf);
  
		/* what URL that receives this POST */
		curl_easy_setopt( m_easyHandle, CURLOPT_URL, m_request.url.c_str() );
		curl_easy_setopt( m_easyHandle, CURLOPT_HTTPHEADER, m_headerlist );
		curl_easy_setopt( m_easyHandle, CURLOPT_HTTPPOST, m_formpost );

		curl_easy_setopt( m_easyHandle, CURLOPT_WRITEFUNCTION, writeRequestPerformerResponse );
        curl_easy_setopt( m_easyHandle, CURLOPT_WRITEDATA, this );

        if( m_errorBuffer != NULL )
        {
            curl_easy_setopt( m_easyHandle, CURLOPT_ERRORBUFFER, m_errorBuffer );
        }
	}
    //////////////////////////////////////////////////////////////////////////
}
