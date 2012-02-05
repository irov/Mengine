#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/PluginInterface.h"

#	define HTTP_MAX_URL_SIZE 255 

namespace Menge
{
	typedef String HTTPStringType;

	enum EHTTPMethod
	{
		HTTPM_GET = 0x00000000,
		HTTPM_POST = 0x00000001
	};
	
	enum EHTTPResponseFlags
	{
		HTTPF_SUCCES = 0x00000001,
        HTTPF_FAILED = 0x00000010,
        HTTPF_COMPLETE = 0x00000100,
        HTTPF_INPROGRESS = 0x00001000
    };
	
	enum EHTTPRequestFlags
	{
		HTTPF_WRITE_RESPONSE = 0x00000000
		//EHTTPF_REPEAT_AFTER_FAIL = 0x00000001
	};
		
	struct HTTPVariable
	{
		/*
		HTTPVariable(){}
		HTTPVariable( const HTTPStringType & _name, const HTTPStringType & _value )
			: name(_name), value(_value)
		{
		}
		*/
		HTTPStringType name;
		HTTPStringType value;
	};
	
	typedef std::vector<HTTPVariable> THTTPVariablesVector;
	
	struct HTTPRequest
	{
		THTTPVariablesVector variables;
		HTTPStringType url;
		EHTTPMethod requestType;
		size_t flags;
	};

	struct HTTPResponse
	{
		HTTPStringType responseData;
		size_t httpCode;
        size_t flags;
	};
	
	class HTTPResponseReceiver
	{
    public:
        virtual void receive( const HTTPResponse & _response ) = 0;
	};
    
    class HTTPSystemInterface
	{
	public:
		virtual void send( const HTTPRequest & _request, HTTPResponseReceiver * _receiver ) = 0;
		virtual void sendAsync( const HTTPRequest& _request, HTTPResponseReceiver * _receiver ) = 0;
		virtual void update() = 0;
	};

	class HTTPServiceInterface
		: public ServiceInterface
	{
	public:
		virtual HTTPSystemInterface* getInterface() = 0;
	};
}

bool	initInterfaceSystem( Menge::HTTPSystemInterface** );
void	releaseInterfaceSystem( Menge::HTTPSystemInterface* );
