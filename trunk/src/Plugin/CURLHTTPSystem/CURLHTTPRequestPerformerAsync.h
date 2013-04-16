#	pragma once

#	include "CURLHTTPRequestPerformer.h"

namespace Menge
{
    /* class is adapter which creates synchronous request and attach it`s handle to non blocking curl_multi interface
		so we called CURLHTTPRequestPerformer::initialise() then  CURLHTTPRequestPerformer::getHandle() and add handle to curl_multi_add_handle
	*/
	class CURLHTTPRequestPerformerAsync
       : public CURLHTTPRequestPerformer
    {
    public:
        CURLHTTPRequestPerformerAsync( const HTTPRequest& _request );
        virtual ~CURLHTTPRequestPerformerAsync();
	protected:
		virtual void _init(void);
        virtual void _execute(void) override;
		virtual void _clear(void);
		CURLM * m_multiHandle;
		CURLHTTPRequestPerformer * m_performer;
	};
}