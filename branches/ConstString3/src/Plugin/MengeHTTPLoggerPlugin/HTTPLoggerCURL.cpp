#	include "HTTPLoggerCURL.h"
#	include "windows.h"
//1 mb buffer
#	define HTTP_LOGGER_CURL_BUFFER_LIMIT 1048576

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static void wstrToUtf8( const Menge::WString& _wstr, Menge::String & _utf8 )
	{
		const wchar_t* cwstr = _wstr.c_str();
		int size = ::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, 0, 0, 0, 0 );
		char * buffer = new char[size];
		::WideCharToMultiByte( CP_UTF8, 0, cwstr, -1, buffer, size, NULL, NULL );
		_utf8.assign( buffer );
		delete [] buffer;
	}
	
	//////////////////////////////////////////////////////////////////////////
	HTTPLoggerCURL::HTTPLoggerCURL( HTTPSystemInterface * _interfaceHTTP ): HTTPLogger( _interfaceHTTP )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HTTPLoggerCURL::~HTTPLoggerCURL()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void HTTPLoggerCURL::log( const void * _data, int _count, EMessageLevel _level )
	{
		if( m_buffer.size() >= HTTP_LOGGER_CURL_BUFFER_LIMIT )
		{
			flush();
		}
		m_buffer += (char*) _data;
	}
	//////////////////////////////////////////////////////////////////////////
	void HTTPLoggerCURL::flush()
	{
		if(m_interface == NULL)
		{
			int x = 1;
		}

		HTTPRequest request;
		request.requestType = HTTPM_POST;
		request.url = "http://argalientertainment.com/menge/receiver.php";
		//request.url = "menge/receiver.php";
		
        HTTPVariable data;
		data.name = "logData";
		data.value = m_buffer;
		request.variables.push_back(data);
        /*
        HTTPVariable login;
		login.name = "login";
        login.value = HTTPStringType("adm");
        request.variables.push_back(login);
        
        HTTPVariable password;
		password.name = "password";
        password.value = HTTPStringType("12345");
        request.variables.push_back(password);
        */
		//Get computer NetBios name
		WCHAR buffer[MAX_COMPUTERNAME_LENGTH+1];
		DWORD size;
		size=sizeof(buffer);
		GetComputerName(buffer,&size);
		WString bufferW(buffer);
		String netBiosName;
		wstrToUtf8( bufferW, netBiosName );

		HTTPVariable pcName;
		pcName.name = "pcName";
		pcName.value = netBiosName;
		request.variables.push_back(pcName);

		m_interface->send( request, this );
		
		m_buffer.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    void HTTPLoggerCURL::receive( const HTTPResponse & _response )
    {
        //HTTPResponse response = _response;
        //const char * str = _response.responseData.c_str();
    }
    //////////////////////////////////////////////////////////////////////////
    
}

