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
        ~CURLHTTPRequestPerformerSync();

    protected:
        void _init() override;		
    };
}