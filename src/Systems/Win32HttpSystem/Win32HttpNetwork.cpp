#include "Win32HttpNetwork.h"

#include "Interface/CryptographySystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/ArrayString.h"
#include "Kernel/HttpLogger.h"
#include "Kernel/Stringalized.h"

#include "Config/Version.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool setTimeout( HINTERNET _hRequest, int _timeout )
            {
                if( _timeout == -1 )
                {
                    return true;
                }

                if( ::InternetSetOption( _hRequest, INTERNET_OPTION_RECEIVE_TIMEOUT, &_timeout, sizeof( _timeout ) ) == FALSE )
                {
                    return false;
                }

                if( ::InternetSetOption( _hRequest, INTERNET_OPTION_SEND_TIMEOUT, &_timeout, sizeof( _timeout ) ) == FALSE )
                {
                    return false;
                }

                if( ::InternetSetOption( _hRequest, INTERNET_OPTION_CONNECT_TIMEOUT, &_timeout, sizeof( _timeout ) ) == FALSE )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool setBasicAuthorization( HINTERNET _hRequest, const String & _login, const String & _password )
            {
                ArrayString<1024> credentials;
                credentials.append( _login );
                credentials.append( ":" );
                credentials.append( _password );

                const Char * credentials_str = credentials.c_str();
                ArrayString<1024>::size_type credentials_size = credentials.size();

                CHAR bstrEncoded[1024];
                DWORD dwSize = 1024;
                ::CryptBinaryToStringA( (const BYTE *)credentials_str, credentials_size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, bstrEncoded, &dwSize );

                ArrayString<1024> header;
                header.append( "Authorization: Basic " );
                header.append( bstrEncoded );

                const Char * header_str = header.c_str();
                ArrayString<1024>::size_type header_size = header.size();

                if( ::HttpAddRequestHeadersA( _hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool setData( HINTERNET _hRequest, const Data & _data )
            {
                const Data::value_type * data_buffer = _data.data();
                Data::size_type data_size = _data.size();

                ArrayString<1024> header;
                header.append( "Content-Length: " );

                Char strContentLength[32] = {'\0'};
                Helper::stringalized( data_size, strContentLength, 32 );

                header.append( strContentLength );

                const Char * header_str = header.c_str();
                ArrayString<1024>::size_type header_size = header.size();

                INTERNET_BUFFERSA buffersIn;
                ::ZeroMemory( &buffersIn, sizeof( INTERNET_BUFFERSA ) );
                buffersIn.dwStructSize = sizeof( INTERNET_BUFFERSA );

                buffersIn.lpcszHeader = (LPCSTR)header_str;
                buffersIn.dwHeadersLength = header_size;

                buffersIn.lpvBuffer = (LPVOID)data_buffer;
                buffersIn.dwBufferLength = data_size;

                if( ::HttpSendRequestExA( _hRequest, &buffersIn, NULL, 0, 0 ) == FALSE )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool setHeaders( HINTERNET _hRequest, const VectorString & _headers )
            {
                for( const String & header : _headers )
                {
                    const Char * header_str = header.c_str();
                    String::size_type header_size = header.size();

                    if( ::HttpAddRequestHeadersA( _hRequest, header_str, (DWORD)header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
                    {
                        return false;
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool setMultipartFormData( HINTERNET _hRequest, const HttpRequestPostProperties & _properties )
            {
                Char boundary[17] = {'\0'};
                CRYPTOGRAPHY_SYSTEM()
                    ->generateRandomHexadecimal( 16, boundary );

                ArrayString<1024> header;
                header.append( "Content-Type: multipart/form-data; boundary=" );
                header.append( boundary );

                const Char * header_str = header.c_str();
                ArrayString<1024>::size_type header_size = header.size();

                if( ::HttpAddRequestHeadersA( _hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
                {
                    return false;
                }

                ArrayString<8096> body;

                for( const HttpRequestPostProperty & pair : _properties )
                {
                    body.append( "--" );
                    body.append( boundary );
                    body.append( "\r\n" );

                    body.append( "Content-Disposition: form-data; name=\"" );
                    body.append( pair.key );
                    body.append( "\"\r\n" );

                    body.append( "Content-Type: text/plain; charset=UTF-8\r\n" );
                    body.append( pair.value );
                    body.append( "\r\n" );
                }

                body.append( "--" );
                body.append( boundary );
                body.append( "--\r\n" );

                const Char * body_str = body.c_str();
                ArrayString<8096>::size_type body_size = body.size();

                INTERNET_BUFFERSA buffersIn;
                ::ZeroMemory( &buffersIn, sizeof( INTERNET_BUFFERSA ) );
                buffersIn.dwStructSize = sizeof( INTERNET_BUFFERSA );
                buffersIn.lpvBuffer = (LPVOID)body_str;
                buffersIn.dwBufferLength = body_size;
                buffersIn.dwBufferTotal = body_size;

                if( ::HttpSendRequestExA( _hRequest, &buffersIn, NULL, 0, 0 ) == FALSE )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static void errorRequest( const HttpResponseInterfacePtr & _response )
            {
                DWORD lastError = ::GetLastError();
                MENGINE_UNUSED( lastError );

                if( lastError == ERROR_INTERNET_EXTENDED_ERROR )
                {
                    DWORD errorCode = 0;
                    Char errorMessage[4096] = {'\0'};
                    DWORD bufferLength = sizeof( errorMessage );

                    if( ::InternetGetLastResponseInfoA( &errorCode, errorMessage, &bufferLength ) == FALSE )
                    {
                        DWORD lastError2 = ::GetLastError();
                        MENGINE_UNUSED( lastError2 );

                        //ToDo
                    }

                    _response->setError( errorMessage, errorCode );
                }
                else
                {
                    Char errorMessage[2048] = {'\0'};
                    Helper::Win32ReadErrorMessageA( lastError, errorMessage, 2048 );
                    
                    _response->setError( errorMessage, lastError );
                }

                LOGGER_HTTP_INFO( "http request info: %s [%u]"
                    , _response->getErrorMessage().c_str()
                    , _response->getErrorCode()
                );
            }
            //////////////////////////////////////////////////////////////////////////
            static bool openRequestEx( const HttpRequestInterface * _request, const Char * _verb, HINTERNET * const _hInternet, HINTERNET * const _hConnect, HINTERNET * const _hRequest )
            {
                const String & HTTP_URL = _request->getURL();
                int32_t HTTP_TIMEOUT = _request->getTimeout();
                const HttpRequestHeaders & HTTP_HEADERS = _request->getHeaders();

                ArrayString<1024> agent;
                agent.append( "Mengine" );
                agent.append( "/" );
                agent.append( MENGINE_VERSION_STRING );
                agent.append( " (" );

                Char osFamily[MENGINE_PLATFORM_OS_FAMILY_MAXNAME] = {'\0'};
                PLATFORM_SERVICE()
                    ->getOsFamily( osFamily );

                agent.append( osFamily );

                Char osVersion[MENGINE_PLATFORM_OS_VERSION_MAXNAME] = {'\0'};
                PLATFORM_SERVICE()
                    ->getOsVersion( osVersion );

                agent.append( " " );
                agent.append( osVersion );

                agent.append( ")" );

                HINTERNET hInternet = ::InternetOpenA( agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );

                if( hInternet == NULL )
                {
                    return false;
                }

                URL_COMPONENTSA urlComponents;
                ::ZeroMemory( &urlComponents, sizeof( urlComponents ) );
                urlComponents.dwStructSize = sizeof( urlComponents );

                Char password[256] = {'\0'};
                Char userName[256] = {'\0'};
                Char scheme[256] = {'\0'};
                Char hostName[256] = {'\0'};
                Char urlPath[256] = {'\0'};
                Char extraInfo[256] = {'\0'};

                urlComponents.lpszScheme = scheme;
                urlComponents.dwSchemeLength = sizeof( scheme ) / sizeof( Char );
                urlComponents.lpszHostName = hostName;
                urlComponents.dwHostNameLength = sizeof( hostName ) / sizeof( Char );
                urlComponents.lpszUserName = userName;
                urlComponents.dwUserNameLength = sizeof( userName ) / sizeof( Char );
                urlComponents.lpszPassword = password;
                urlComponents.dwPasswordLength = sizeof( password ) / sizeof( Char );
                urlComponents.lpszUrlPath = urlPath;
                urlComponents.dwUrlPathLength = sizeof( urlPath ) / sizeof( Char );
                urlComponents.lpszExtraInfo = extraInfo;
                urlComponents.dwExtraInfoLength = sizeof( extraInfo ) / sizeof( Char );

                if( ::InternetCrackUrlA( HTTP_URL.c_str(), HTTP_URL.size(), 0, &urlComponents ) == FALSE )
                {
                    return false;
                }

                HINTERNET hConnect = ::InternetConnectA( hInternet, urlComponents.lpszHostName, urlComponents.nPort, userName, password, INTERNET_SERVICE_HTTP, 0, 0 );

                if( hConnect == NULL )
                {
                    return false;
                }

                DWORD dwFlags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES;
                
                if( urlComponents.nScheme == INTERNET_SCHEME_HTTPS )
                {
                    dwFlags |= INTERNET_FLAG_SECURE;
                }

                HINTERNET hRequest = ::HttpOpenRequestA( hConnect, _verb, urlComponents.lpszUrlPath, "HTTP/1.1", NULL, NULL, dwFlags, 0 );

                if( hRequest == NULL )
                {
                    return false;
                }

                DWORD securityFlags = 0;
                DWORD securityFlagsLen = sizeof( securityFlags );
                if( ::InternetQueryOptionA( hRequest, INTERNET_OPTION_SECURITY_FLAGS, (void *)&securityFlags, &securityFlagsLen ) == FALSE )
                {
                    return false;
                }

                securityFlags |= SECURITY_FLAG_IGNORE_REVOCATION;
                securityFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;

                if( ::InternetSetOptionA( hRequest, INTERNET_OPTION_SECURITY_FLAGS, &securityFlags, sizeof( securityFlags ) ) == FALSE )
                {
                    return false;
                }

                if( Detail::setTimeout( hRequest, HTTP_TIMEOUT ) == false )
                {
                    return false;
                }

                if( Detail::setHeaders( hRequest, HTTP_HEADERS ) == false )
                {
                    return false;
                }

                *_hInternet = hInternet;
                *_hConnect = hConnect;
                *_hRequest = hRequest;

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool openRequest( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const Char * _verb, HINTERNET * const _hInternet, HINTERNET * const _hConnect, HINTERNET * const _hRequest )
            {
                if( Detail::openRequestEx( _request, _verb, _hInternet, _hConnect, _hRequest ) == true )
                {
                    return true;
                }

                Detail::errorRequest( _response );

                if( *_hRequest != NULL )
                {
                    ::InternetCloseHandle( *_hRequest );
                }

                if( *_hConnect != NULL )
                {
                    ::InternetCloseHandle( *_hConnect );
                }

                if( *_hInternet != NULL )
                {
                    ::InternetCloseHandle( *_hInternet );
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getResponseStatusCode( HINTERNET _hRequest, const HttpResponseInterfacePtr & _response )
            {
                DWORD statusCode = 0;
                DWORD bufferSize = sizeof( DWORD );
                if( ::HttpQueryInfoA( _hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &bufferSize, NULL ) == FALSE )
                {
                    return false;
                }

                _response->setCode( (EHttpCode)statusCode );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getResponseData( HINTERNET _hRequest, const HttpResponseInterfacePtr & _response )
            {
                for( ;; )
                {
                    DWORD bytesAvailable = 0;
                    if( ::InternetQueryDataAvailable( _hRequest, &bytesAvailable, 0, 0 ) == FALSE )
                    {
                        return false;
                    }

                    if( bytesAvailable == 0 )
                    {
                        break;
                    }

                    DWORD bytesRequest = bytesAvailable > 4096 ? 4096 : bytesAvailable;

                    Char buffer[4096] = {'\0'};
                    DWORD bytesRead = 0;
                    if( ::InternetReadFile( _hRequest, buffer, bytesRequest, &bytesRead ) == FALSE )
                    {
                        return false;
                    }

                    if( bytesRead == 0 )
                    {
                        break;
                    }

                    _response->appendData( buffer, bytesRead );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool makeResponse( HINTERNET _hRequest, const HttpResponseInterfacePtr & _response )
            {
                if( ::HttpSendRequestExA( _hRequest, NULL, NULL, 0, 0 ) == FALSE )
                {
                    return false;
                }

                if( ::HttpEndRequestA( _hRequest, NULL, 0, 0 ) == FALSE )
                {
                    return false;
                }

                if( Detail::getResponseData( _hRequest, _response ) == false )
                {
                    return false;
                }

                if( Detail::getResponseStatusCode( _hRequest, _response ) == false )
                {
                    return false;
                }

                LOGGER_HTTP_INFO( "response [%u] data: %s"
                    , _response->getRequestId()
                    , _response->getData().c_str()
                );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestPostMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const HttpRequestPostProperties & _properties )
        {
            LOGGER_HTTP_INFO( "post message url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hInternet = NULL;
            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "POST", &hInternet, &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::setMultipartFormData( hRequest, _properties ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            if( Detail::makeResponse( hRequest, _response ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );
            ::InternetCloseHandle( hInternet );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestHeaderData( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const Data & _data )
        {
            LOGGER_HTTP_INFO( "header data url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hInternet = NULL;
            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "POST", &hInternet, &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::setData( hRequest, _data ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            if( Detail::makeResponse( hRequest, _response ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );
            ::InternetCloseHandle( hInternet );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestGetMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response )
        {
            HINTERNET hInternet = NULL;
            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "GET", &hInternet, &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::makeResponse( hRequest, _response ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );
            ::InternetCloseHandle( hInternet );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestDeleteMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response )
        {
            LOGGER_HTTP_INFO( "delete message url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hInternet = NULL;
            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "DELETE", &hInternet, &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::makeResponse( hRequest, _response ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );
            ::InternetCloseHandle( hInternet );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestGetAsset( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const String & _login, const String & _password )
        {
            LOGGER_HTTP_INFO( "get asset url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hInternet = NULL;
            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "GET", &hInternet, &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::setBasicAuthorization( hRequest, _login, _password ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            if( Detail::makeResponse( hRequest, _response ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );
                ::InternetCloseHandle( hInternet );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );
            ::InternetCloseHandle( hInternet );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}