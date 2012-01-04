#	pragma once

#	include "CURLHTTPRequestPerformer.h"

namespace Menge
{
    //performs simply synchronous request
	class CURLHTTPRequestPerformerSync
       : public CURLHTTPRequestPerformer
    {
    public:
        CURLHTTPRequestPerformerSync( const HTTPRequest& _request );
        virtual ~CURLHTTPRequestPerformerSync();
    protected:
        virtual void _init() override;
		
    };
}