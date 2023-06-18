#include "cURLHttpErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLHttpErrorHelper::cURLHttpErrorHelper( const Char * _file, uint32_t _line, const Char * _method )
        : m_file( _file )
        , m_line( _line )
        , m_method( _method )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpErrorHelper::operator == ( CURLcode _code ) const
    {
        if( _code == CURLE_OK )
        {
            return false;
        }

        LOGGER_VERBOSE_LEVEL( "curl", LM_ERROR, LFILTER_NONE, LCOLOR_RED, m_file, m_line, LFLAG_FULL )("call '%s' get error '%s' [%u]"
            , m_method
            , curl_easy_strerror( _code )
            , _code
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}