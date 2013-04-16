#	pragma once

#	include "CURLHTTPRequestPerformer.h"

namespace Menge
{
    //Simulate sending form on server
	class CURLHTTPRequestPerformerPostForm
       : public CURLHTTPRequestPerformer
    {
    public:
        CURLHTTPRequestPerformerPostForm( const HTTPRequest& _request );
        virtual ~CURLHTTPRequestPerformerPostForm();

    protected:
        virtual void _init() override;
		virtual void _clear() override;
		struct curl_httppost * m_formpost;
		struct curl_slist * m_headerlist;
    };
}