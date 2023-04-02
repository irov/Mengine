#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

#include "curl/curl.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_CURL_CHECK_ERROR
#   if defined(MENGINE_DEBUG)
#       define MENGINE_CURL_CHECK_ERROR 1
#   else
#       define MENGINE_CURL_CHECK_ERROR 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_CURL_CHECK_ERROR == 1
#   define MENGINE_CURL_CHECK_ERROR_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    class cURLErrorHelper
    {
    public:
        cURLErrorHelper( const Char * _file, uint32_t _line, const Char * _method );

    public:
        bool operator == ( CURLcode _code ) const;

    protected:
        const Char * m_file;
        uint32_t m_line;
        const Char * m_method;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_CURLERRORCHECK( Method, HRES )\
    (Mengine::cURLErrorHelper(MENGINE_CODE_FILE, MENGINE_CODE_LINE, Method ) == HRES)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_CURLERRORCHECK( Method, HRES )\
    if( MENGINE_CURLERRORCHECK(#Method, HRES) )
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_CURL_CHECK_ERROR_ENABLE)
#   define MENGINE_CURLCALL( Method, Args )\
    (MENGINE_CURLERRORCHECK(#Method, Method Args))
#else
#   define MENGINE_CURLCALL( Method, Args )\
    (Method Args)
#endif
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_CURLCALL( Method, Args )\
    if( MENGINE_CURLCALL(Method, Args) )
//////////////////////////////////////////////////////////////////////////