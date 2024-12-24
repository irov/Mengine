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
                if( _timeout == MENGINE_HTTP_REQUEST_TIMEOUT_INFINITY )
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
            static bool setHeaders( HINTERNET _hRequest, const HttpHeaders & _headers )
            {
                for( const String & header : _headers )
                {
                    const Char * header_str = header.c_str();
                    String::size_type header_size = header.size();

                    if( ::HttpAddRequestHeadersA( _hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
                    {
                        return false;
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static void errorRequest( const HttpResponseInterfacePtr & _response )
            {
                DWORD lastError = ::GetLastError();
                MENGINE_UNUSED( lastError );

                if( lastError == ERROR_INTERNET_TIMEOUT )
                {
                    _response->setError( "http request timeout", lastError );
                }
                else if( lastError == ERROR_INTERNET_INCORRECT_HANDLE_STATE )
                {
                    _response->setError( "http request incorrect handle state", lastError );
                }
                else if( lastError == ERROR_INTERNET_EXTENDED_ERROR )
                {
                    DWORD errorCode = 0;
                    Char errorMessage[4096 + 1] = {'\0'};
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
                    Char errorMessage[2048 + 1] = {'\0'};
                    Helper::Win32ReadErrorMessageA( lastError, errorMessage, 2048 );

                    _response->setError( errorMessage, lastError );
                }

                LOGGER_HTTP_INFO( "http request info: %s [%u]"
                    , _response->getErrorMessage().c_str()
                    , _response->getErrorCode()
                );
            }
            //////////////////////////////////////////////////////////////////////////
            static bool openRequest( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const Char * _verb, HINTERNET * const _hConnect, HINTERNET * const _hRequest )
            {
                HINTERNET hInternet = _request->getHInternet();

                URL_COMPONENTSA urlComponents;
                ::ZeroMemory( &urlComponents, sizeof( urlComponents ) );
                urlComponents.dwStructSize = sizeof( urlComponents );

                Char password[1024 + 1] = {'\0'};
                Char userName[1024 + 1] = {'\0'};
                Char scheme[1024 + 1] = {'\0'};
                Char hostName[1024 + 1] = {'\0'};
                Char urlPath[1024 + 1] = {'\0'};
                Char extraInfo[1024 + 1] = {'\0'};

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

                const String & HTTP_URL = _request->getURL();

                const Char * HTTP_URL_str = HTTP_URL.c_str();
                size_t HTTP_URL_size = HTTP_URL.size();

                if( ::InternetCrackUrlA( HTTP_URL_str, HTTP_URL_size, 0, &urlComponents ) == FALSE )
                {
                    Detail::errorRequest( _response );

                    return false;
                }

                DWORD dwConnectFlags = 0;

                HINTERNET hConnect = ::InternetConnectA( hInternet
                    , urlComponents.lpszHostName
                    , urlComponents.nPort
                    , urlComponents.lpszUserName
                    , urlComponents.lpszPassword
                    , INTERNET_SERVICE_HTTP
                    , dwConnectFlags
                    , INTERNET_NO_CALLBACK );

                if( hConnect == NULL )
                {
                    Detail::errorRequest( _response );

                    return false;
                }

                DWORD dwRequestFlags = 0;
                dwRequestFlags |= INTERNET_FLAG_RELOAD;
                dwRequestFlags |= INTERNET_FLAG_KEEP_CONNECTION;
                dwRequestFlags |= INTERNET_FLAG_NO_AUTH;
                dwRequestFlags |= INTERNET_FLAG_NO_CACHE_WRITE;
                dwRequestFlags |= INTERNET_FLAG_NO_COOKIES;
                dwRequestFlags |= INTERNET_FLAG_NO_UI;

                if( urlComponents.nScheme == INTERNET_SCHEME_HTTPS )
                {
                    dwRequestFlags |= INTERNET_FLAG_SECURE;

                    dwRequestFlags |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP;
                    dwRequestFlags |= INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
                    dwRequestFlags |= INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
                    dwRequestFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
                }

                HINTERNET hRequest = ::HttpOpenRequestA( hConnect
                    , _verb
                    , urlComponents.lpszUrlPath
                    , NULL
                    , NULL
                    , NULL
                    , dwRequestFlags
                    , INTERNET_NO_CALLBACK );

                if( hRequest == NULL )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hConnect );

                    return false;
                }

                DWORD securityFlags = 0;
                DWORD securityFlagsLen = sizeof( securityFlags );
                if( ::InternetQueryOptionA( hRequest, INTERNET_OPTION_SECURITY_FLAGS, (void *)&securityFlags, &securityFlagsLen ) == FALSE )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hConnect );
                    ::InternetCloseHandle( hRequest );

                    return false;
                }

                securityFlags |= SECURITY_FLAG_IGNORE_REVOCATION;
                securityFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
                securityFlags |= SECURITY_FLAG_IGNORE_WRONG_USAGE;
                securityFlags |= SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
                securityFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

                if( ::InternetSetOptionA( hRequest, INTERNET_OPTION_SECURITY_FLAGS, &securityFlags, sizeof( securityFlags ) ) == FALSE )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hConnect );
                    ::InternetCloseHandle( hRequest );

                    return false;
                }

                int32_t HTTP_TIMEOUT = _request->getTimeout();

                if( Detail::setTimeout( hRequest, HTTP_TIMEOUT ) == false )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hConnect );
                    ::InternetCloseHandle( hRequest );

                    return false;
                }

                const HttpHeaders & HTTP_HEADERS = _request->getHeaders();

                if( Detail::setHeaders( hRequest, HTTP_HEADERS ) == false )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hConnect );
                    ::InternetCloseHandle( hRequest );

                    return false;
                }

                *_hConnect = hConnect;
                *_hRequest = hRequest;

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool sendRequest( HINTERNET _hRequest )
            {
                if( ::HttpSendRequestExA( _hRequest, NULL, NULL, 0, 0 ) == FALSE )
                {
                    return false;
                }

                return true;
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
            static bool getResponseData( HINTERNET _hRequest, const HttpResponseInterfacePtr & _response, bool _isJSON )
            {
                for( ;; )
                {
                    DWORD bytesAvailable = 0;
                    if( ::InternetQueryDataAvailable( _hRequest, &bytesAvailable, 0, INTERNET_NO_CALLBACK ) == FALSE )
                    {
                        return false;
                    }

                    if( bytesAvailable == 0 )
                    {
                        break;
                    }

                    DWORD bytesRequest = bytesAvailable > 4096 ? 4096 : bytesAvailable;

                    BYTE buffer[4096 + 1] = {'\0'};
                    DWORD bytesRead = 0;
                    if( ::InternetReadFile( _hRequest, buffer, bytesRequest, &bytesRead ) == FALSE )
                    {
                        return false;
                    }

                    if( bytesRead == 0 )
                    {
                        break;
                    }

                    if( _isJSON == true )
                    {
                        _response->appendJSON( buffer, bytesRead );
                    }
                    else
                    {
                        _response->appendData( buffer, bytesRead );
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool makeResponseData( HINTERNET _hRequest, const HttpResponseInterfacePtr & _response, bool _isJSON )
            {
                if( ::HttpEndRequestA( _hRequest, NULL, 0, INTERNET_NO_CALLBACK ) == FALSE )
                {
                    return false;
                }

                if( Detail::getResponseStatusCode( _hRequest, _response ) == false )
                {
                    return false;
                }

                if( Detail::getResponseData( _hRequest, _response, _isJSON ) == false )
                {
                    return false;
                }

                LOGGER_HTTP_INFO( "response [%u] code: %u data: %zu"
                    , _response->getRequest()->getRequestId()
                    , _response->getCode()
                    , _response->getData().size()
                );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestPing( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response )
        {
            LOGGER_HTTP_INFO( "ping url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "GET", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, true ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestPostMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const HttpRequestPostProperties & _properties )
        {
            LOGGER_HTTP_INFO( "post message url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "POST", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            Char boundary[16 + 1] = {'\0'};
            CRYPTOGRAPHY_SYSTEM()
                ->generateRandomHexadecimal( 16, boundary, false );

            ArrayString<1024> header;
            header.append( "Content-Type: multipart/form-data; boundary=" );
            header.append( boundary );

            const Char * header_str = header.c_str();
            ArrayString<1024>::size_type header_size = header.size();

            if( ::HttpAddRequestHeadersA( hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

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

            DWORD dwNumberOfBytesWritten = 0;
            DWORD dwNumberOfBytesToWrite = (DWORD)body_size;

            if( ::InternetWriteFile( hRequest, (LPCVOID)body_str, dwNumberOfBytesToWrite, &dwNumberOfBytesWritten ) == FALSE )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, true ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestHeaderData( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const Data & _data )
        {
            LOGGER_HTTP_INFO( "header data url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "POST", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            const Data::value_type * data_buffer = _data.data();
            Data::size_type data_size = _data.size();

            ArrayString<1024> header;
            header.append( "Content-Length: " );

            Char strContentLength[32 + 1] = {'\0'};
            Helper::stringalized( data_size, strContentLength, 32 );

            header.append( strContentLength );

            const Char * header_str = header.c_str();
            ArrayString<1024>::size_type header_size = header.size();

            if( ::HttpAddRequestHeadersA( hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            DWORD dwNumberOfBytesWritten = 0;
            DWORD dwNumberOfBytesToWrite = (DWORD)data_size;

            if( ::InternetWriteFile( hRequest, (LPCVOID)data_buffer, dwNumberOfBytesToWrite, &dwNumberOfBytesWritten ) == FALSE )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, true ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestGetMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response )
        {
            LOGGER_HTTP_INFO( "get message url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "HEAD", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, true ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestDeleteMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response )
        {
            LOGGER_HTTP_INFO( "delete message url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "DELETE", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, true ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool httpRequestGetAsset( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const String & _login, const String & _password )
        {
            LOGGER_HTTP_INFO( "get asset url: %s [%u]"
                , _request->getURL().c_str()
                , _request->getRequestId()
            );

            HINTERNET hConnect = NULL;
            HINTERNET hRequest = NULL;
            if( Detail::openRequest( _request, _response, "GET", &hConnect, &hRequest ) == false )
            {
                return false;
            }

            if( _login.empty() == false && _password.empty() == false )
            {
                ArrayString<1024> credentials;
                credentials.append( _login );
                credentials.append( ":" );
                credentials.append( _password );

                const Char * credentials_str = credentials.c_str();
                ArrayString<1024>::size_type credentials_size = credentials.size();

                CHAR bstrEncoded[1024 + 1] = {'\0'};
                DWORD dwSize = 1024;
                ::CryptBinaryToStringA( (const BYTE *)credentials_str, credentials_size, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, bstrEncoded, &dwSize );

                ArrayString<1024> header;
                header.append( "Authorization: Basic " );
                header.append( bstrEncoded );

                const Char * header_str = header.c_str();
                ArrayString<1024>::size_type header_size = header.size();

                if( ::HttpAddRequestHeadersA( hRequest, header_str, header_size, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE ) == FALSE )
                {
                    Detail::errorRequest( _response );

                    ::InternetCloseHandle( hRequest );
                    ::InternetCloseHandle( hConnect );

                    return false;
                }
            }

            if( Detail::sendRequest( hRequest ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            if( Detail::makeResponseData( hRequest, _response, false ) == false )
            {
                Detail::errorRequest( _response );

                ::InternetCloseHandle( hRequest );
                ::InternetCloseHandle( hConnect );

                return false;
            }

            ::InternetCloseHandle( hRequest );
            ::InternetCloseHandle( hConnect );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}