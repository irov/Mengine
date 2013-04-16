#	pragma once

#	include <curl/curl.h>
#	include <curl/types.h>
#	include <curl/easy.h>

#	include "Interface/HTTPSystemInterface.h"

namespace Menge
{
    /*template <class T>
    size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        T * perfomer = static_cast<T*>(userdata);
        perfomer->writeToResponse( ptr, size, nmemb );
        return size * nmemb;
    }*/

    //write callback to curl IO
	size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata);
	
	//provide curl sending request action
    class CURLHTTPRequestPerformer
    {
    public:
        CURLHTTPRequestPerformer( const HTTPRequest & _request );
        virtual ~CURLHTTPRequestPerformer();

    public: 
        void addReceiver( HTTPResponseReceiver * _receiver );
        void setErrorBuffer( char * _buffer );
        virtual void execute();
        bool isComplete() const;
		const HTTPResponse & getResponse() const;
        CURL * getEasyHandle() const;
		void initialise();
		friend size_t writeRequestPerformerResponse( char *ptr, size_t size, size_t nmemb, void *userdata);

	protected:
        virtual void _sendToReceivers();
		virtual void _writeToResponse( char *ptr, size_t size, size_t nmemb );
		virtual void _execute();
		virtual void _init() = 0;
		virtual void _clear();
        
		HTTPRequest  m_request;
        HTTPResponse m_response;
        typedef std::vector<HTTPResponseReceiver *> THTTPReceiversVector;
        THTTPReceiversVector m_receivers;
        char * m_errorBuffer;

		CURL * m_easyHandle;
    };
}
